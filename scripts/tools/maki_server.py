import argparse
from http.server import SimpleHTTPRequestHandler, HTTPServer
from functools import partial
from pathlib import Path

class CustomHandler(SimpleHTTPRequestHandler):
    def end_headers(self):
        self.send_header("Cross-Origin-Opener-Policy", "same-origin")
        self.send_header("Cross-Origin-Embedder-Policy", "require-corp")
        super().end_headers()

    def do_GET(self):
        if self.path == "/":
            self.path = "/maki.html"
        return super().do_GET()

PORT = 8080

parser = argparse.ArgumentParser(description='Deploy server to run wasm MAKI')
parser.add_argument(
    '--folder',
    type=Path,
    default="./build/wasm/maki",
    help='Folder containing the MAKI application',
)
parser.add_argument(
    '--host',
    type=str,
    default='localhost',
    help='Host name, default: localhost',
)
parser.add_argument(
    '--port',
    type=int,
    default=8080,
    help='Port to listen on, default: 8080',
)

if __name__ == "__main__":
  args = parser.parse_args()

  Handler = partial(CustomHandler, directory=args.folder)
  with HTTPServer((args.host, args.port), Handler) as httpd:
      print(f'Serving {args.folder} at http://{args.host}:{args.port}')
      httpd.serve_forever()
