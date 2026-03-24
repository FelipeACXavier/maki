# KODA plugin

This is the plugin for the KODA language

To generate the parser file, you can run:
```bash
java -jar ./antlr-4.10.1-complete.jar -Dlanguage=Cpp Koda.g4 -no-listener -visitor -o ./parser
```