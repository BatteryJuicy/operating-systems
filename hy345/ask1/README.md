### Overview
This shell reads multiple commands and stores each command (seperated by <;>) in a linked list where each node contains:
  - A string of the whole command with null terminators between each argument/flag
  - an array of char* to each string above
For each command:
  - It **preproccesses** the command to dereference variables if <$>.
  - It looks for <=> to **define a variable**.
  - Checks and **executes built-in** command (cd, exit, export).
  - If it's a **pipeline** (contains |), then it creates a _new_ linked list to seperate each command along <|>.
    (works with and without space between commands and <|>). It then _creates pipes_ and for _each child_ process, _redirects I/O_ to either a _file_
    (if it's the first or the last of the pipe according to what redirection flag was used) or the _previous_ and/or _next_ _command_ in the pipeline.
    Finally it executes each command in the pipeline.
  - If it's **not a pipeline** it just _redirects I/O_ and _executes_ the command.
At the end of the command parsing it frees the cmd linked list before repeating the whole process again.

### Notes
  - Command preproccessing creates a memory leak if the variables are **not** seperated by spaces (e.g. $a$b)
  - The shell **ignores ctrl+c** in the parent process. To exit type ```exit```


