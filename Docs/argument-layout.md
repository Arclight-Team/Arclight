# Argument Layout Language

The class `ArgumentLayout` is responsible for building an expression tree given a layout string for the `ArgumentParser` parser.

The layout follows a specific language to encode the command line arguments compatible with the application.


## Details

Internally, `ArgumentLayout` creates an *node tree*, which is why we refer to the main components of a layout string as *nodes*.

While `ArgumentLayout` allows retrieving the root node of the tree with `getRootNode`, it is not constructible with anything but a layout string to avoid any illegal nodes that would probably cause `ArgumentParser` to misbehave. 


## Basic information

A layout string is composed of operators and nodes, these last ones can be of two kinds:

- Argument: Single argument getting passed to the application from command line
- Parent: "Container" for other argument or parent nodes to create more complex expressions

Each node kind can be of these types:

- `< ... >` *Mandatory*: Passing from command line the contents is mandatory, thus the parser will throw if no matches are found, *unless operators give alternatives*
- `[ ... ]` *Optional*: Passing from command line the contents is optional
- `{ ... }` *Random Access* (Parents only): Argument nodes inside these nodes can be passed in any order


## Argument Nodes

In most applications command line arguments are composed of 3 parts.

Considering `app.exe --input="file.txt"`:

- `--input` is the argument name
- `=` is the separator character
- `"file.txt"` is the value of the argument `--input`

In ArgumentLayout's language, this single argument would most likely translate to:

`< -i | --input = string >`

Generally, there aren't many restrictions and arguments can be heavily customized:

`NodeBegin [ UnnamedFlag ] Name [ | Name ... ] Separator Type NodeEnd`

- `NodeBegin`: Node begin character, `<` (Mandatory) or `[` (Optional), as stated before Random Access nodes cannot be argument nodes
- `UnnamedFlag`: Optional `#` marking the argument as Unnamed, further information below
- `Name`: Argument name, optionally more can be specified by separating them with the Or operator `|`
- `Separator`: Character separating the argument name from its value, must be ` `,
`:` or `=`
- `Type`: Argument type, further information below
- `NodeEnd`: Matching node end character (`>` or `]`)

The additional spacing is optional, thus `<-t|--test word>` and `< -t | --test word >` behave the same.

### Unnamed arguments

Unnamed arguments do not accept a name when getting passed (and thus no separator can be specified either) but only the value alone.

At least a single name is still required for gathering its value.

Unnamed arguments cannot be contained inside Random Access parent nodes, for further information check the *Random Access parents* section.

The parser reads the value alone and does not check whenever the argument name has been accidentally passed.

### Argument types

| Type     | C++ Type    | Description            |
|----------|-------------|------------------------|
| `uint`   | u64         | Unsigned integer       |
| `int`    | s64         | Signed integer         |
| `float`  | float       | Single precision float |
| `double` | double      | Double precision float |
| `string` | std::string | String                 |
| `word`   | std::string | Spaceless string       |

If no Type is specified, the argument will become a Flag.

Flag arguments consist only of their name(s) and so they are either passed (forced if Mandatory) or not passed, useful for representing booleans.

Due to their nature, no separator can be specified and they cannot be marked as Unnamed.


## Operators

The layout language offers two binary operators to write proper expressions:

- `&` `,` And: Specifies an additional required node
- `|` Or: Specifies an alternative to the previous node

The Or operator has always higher priority.

Respecting these rules, the pseudo expression `A | B | C & D & E | F` would read as it follows:

`A, B or C; Then D; Then E or F`

### Optional nodes behaviour

Considering the case `[ -a ] | < -b >`:

If `-a` is not passed, no error is thrown as it is optional.

However, since there may be alternatives (`-b`), the parser sets itself to a "partial match" state where no error will be thrown, but the search for a complete match (which could be `-b`) is still performed.

The exact same behaviour applies when the two operands are parent nodes.


## Parent Nodes

Parent nodes are nodes that group other argument and/or parent together to create complex expressions.

`NodeBegin Contents NodeEnd`

- `NodeBegin`: Node begin character, `<` (Mandatory), `[` (Optional) or `{` (Random Access)
- `Contents`: Contents of the parent, any expression consisting of one or more nodes of any kind with some restrictions for *Random Access* parents
- `NodeEnd`: Matching node end character (`>`, `]` or `}`)

In most cases the same parser behaviour can be achieved with different layouts by reordering the nodes and expressions, however layout string readability should always be prioritized.

### Random Access parents

Random Access parent nodes act differently compared to Mandatory and Optional parent nodes.

Argument nodes inside Random Access parents may be passed to the application in any order, useful for non-ordered flags.

They can only contain an expression between *Named* Argument nodes without Or operators, otherwise the behaviour would be hard to deduce. 


## Examples

### Single argument

`< -a | --arclight | /Arclight word >`

- [x] `app.exe /Arclight Hello`
- [x] `app.exe --arclight "ThisIsAWord"`
- [ ] `app.exe arc-light Thing` (`arc-light` is not a valid argument name)
- [ ] `app.exe -a "This is a string"` (Argument value is not a `word`)

`< # MyArgument double >`

- [x] `app.exe 4.123`
- [x] `app.exe 3`
- [ ] `app.exe MyArgument 5.6` (`MyArgument` is not recognized, argument is unnamed)

`< /File = string >`

- [x] `app.exe /File=README.md`
- [x] `app.exe /File="my folder/my file.bin"`
- [ ] `app.exe /File image.png` (Argument is not recognized, separator missing)

### Multiple arguments (operators)

TODO

`< -i | --input string > & [ -o | --output string ]`

`< -a > & < -b > | [ -c ]`

### Parents

TODO

`< --input string > & < < --import > & < --target string > > | [ < --export > & < --index uint > ]`

`< # File string > & < < --all > | { < -a > & < -b > & < -c > } > | [ --noflags ]`

### Complex layouts

TODO
