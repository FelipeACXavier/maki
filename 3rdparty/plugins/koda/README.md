# KODA plugin

This is the plugin for the KODA language

To generate the parser file, you can run:
```bash
java -jar ./antlr-4.10.1-complete.jar -Dlanguage=Cpp Koda.g4 -no-listener -visitor -o ./parser
```

To start the Dezyne daemon:
```bash
node ./apps/daemon/dist/daemon.cjs --test-run
```

To start the simulation, for example:
```bash
DIR=`pwd`/../../maki/generated/; node ./apps/daemon-cli/dist/main.cjs simulate -I $DIR/lib/ -I $DIR/pick/ $DIR/pick/pickanddrop_task.dzn
```