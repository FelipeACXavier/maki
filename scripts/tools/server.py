from http.server import SimpleHTTPRequestHandler, HTTPServer
from functools import partial
import sys

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

build_dir = sys.argv[1]
Handler = partial(CustomHandler, directory=build_dir)
with HTTPServer(("", PORT), Handler) as httpd:
    print(f"Serving {build_dir}")
    print(f"http://localhost:{PORT}")
    httpd.serve_forever()