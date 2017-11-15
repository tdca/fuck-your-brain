Enhanced brain-fuck
===============

Try Brainfuck Interpreter.

## INSTALL

```bash
make env
make

##try interactive console
./bfi
```

## BASIC USAGE

this example prints number 0 ~ 9

```
++++++++
[
	>++++++<-
]
++++++++++
[
	>.+<-
]
```

and this dose same thing:

```
++[>++++<-]>[>>+++[<++>-]<<-]++++++++++[>.+<-]
```

## EXTENSIONS

`bfi` has several extensions to make `Brainf**k` more `f**kable`.

#### functions

```
$begin 
	[-]
end;

$begin
	> *!
	> *!
	< ++++++++
	[
		> ++++++
		< -
	]
	<
	[
		>> +
		<< -
	]
	>> .
end;
++++[>++++<-]>+ **!
```

displays `A`.

`begin` and `end` is just for readable.


#### literals

alternative way to print `0` ~ `9`

```
+%10 count
>+%47 ascii offset
<
[
	> +. increase and output
	< -  count down
]
```

#### stacks

```
+%73.>+%110.>+%112.>+%117.>+%116.> Input
+%32.>
+%121.>+%111.>+%117.>+%114.>       your
+%32.>
+%110.>+%97.>+%109.>+%101.>+%58.>  name
+%32.>
&  push data pointer stack
,-%10[+%10>,-%10]+%10>[-]>
+%42.>+%42.>+%32.>
+%87.>+%101.>+%108.>+%99.>+%111.>+%109.>+%101.> Welcome
+%32.>
@  pop data pointer stack
[.>]
```

which acts:

```
$ ./bfi example/your-name.bf
Input your name: Kimmy Leo<ENTER>
** Welcome Kimmy Leo
```

