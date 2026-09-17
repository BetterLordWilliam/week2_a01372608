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

## Running the Program


## Program Description




