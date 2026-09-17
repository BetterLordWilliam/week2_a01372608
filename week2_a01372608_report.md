# Week2 Lab Report

Will Otterbein, A01372608

## Summary

This report describes my implementation of the week2 polling / event loop lab.

## Obtaining a Program Binary

Information regarding the compiling of this lab into a binary.

For this lab I have created a [`makefile`](./makefile).

You can run `make` & it will output the binary into a `build` directory.
The name of the binary is `rvloop`.

There are make targets, run is dependent on compilation, so if you execute the
following command in your shell

```bash
make run
```

Make will take care of compiling & running the binary.

> this assumes you have make installed already on the system as well as the
> `gcc` compiler, as the makefile expects the `gcc` compiler & uses it for the
> make commands or recipies

To manually compile with `gcc`, the following command or similar should suffice

```bash
# folder & file are simply the names suggested in the lab instructions
gcc ./event-loop/my_event_loop.c -o my-bin
```

## Using the Program

Once a binary has been obtained & the program is running:

1. You will be greated with a blue message that states "PROGRAM START"

![programstartmsg](./screenshots/progstart.png)

2. From this point onwards the event loop is active, so if you are not inputting
things expect poll messages to be logged every 3 seconds

![polltimeoutmsg](./screenshots/polltimeoutmsg.png)

3. The program is polling for input from stdin, so type some stuff & press enter.
4. Your inputs should be printed to stdout, with an echo prefix.

![inputecho](./screenshots/inputecho.png)

5. after you have had your fun of echoing your own inputs, there are two means
of exiting the program.

6. exit via delivering the EOF with ctrl+d

![ctrldexit](./screenshots/ctrldexit.png)

7. or, exit via typing exit & pressing enter

![exitcommand](./screenshots/exitcommand.png)

> as you can see, different messages are logged for the different program exit
> routes.

8. finally you will see another blue message stating "PROGRAM END"

![programendmsg](./screenshots/progend.png)

Thats the program! The next section describes the implementation.

## Implementation Description

### Use of `POLL`



