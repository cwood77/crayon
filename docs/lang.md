## Overall Concepts
- Script **files** typically end in `.cray`.  Additionally, the interpreter automatically includes any `.crayi` files found anywhere in the enclosing directory _tree_, from root to leaf.  This is so you can define common constants, for example, in a root folder and leverage them automatically in subfolders.
- **Comments** begin with the hash symbol (`#`) and extend to the end of the line
- Many statements take arguments delimted with **spaces**.  If an argument itself requires a space, enclose it in double quotes (`"`).
- Many statements begin **blocks**, which end in a colon (`:`) and are indented.  Indentation is required (i.e. it is syntactically meaningful).
- **Variable expansion** is liberal and essentially follows rules of string substitution. When referenced, variables begin with a dollar sign (`$`).  You may embed a variable reference inside an argument by enclosing the reference in braces (e.g. `{}`), for example: `echo "hello {$name}"`.
- Non-absolute **paths** in script are expected to be relative to the current directory. Alternatively, you may indicate a path is relative to the referencing script by quoting the path and prefixing it with an `r` as in `r"dir/file.cray"`.

## Reference

### Working with Variables

#### `define`: Define a constant

`define varName = value`

- Any variable may be referenced by subsequent code (i.e. there is no scope)
- Defining a new variable of the same name will replace an older variable
- Note that dollar signs are _never_ used in variable declaration, only in reference

#### `sweep`: Iterate over sequence of values

`sweep type start stopOp stopVal delta -> varName :`

for example:

`sweep real 0.88 < 1.0 +0.05 -> i :`

- `type` may be `int` or `real` only
- `stopOp` may be `<` or `>` only
- all values are signed

#### `foreach-stringset`: Parse a text file, and iterate over results

`foreach-stringset path schema -> varName :`

for example:

`foreach-stringset r"path" {a,b} -> str :`

- `schema` is a brace- and comma-delimited sequence of tags to search for in the text.
- the block started by `foreach-stringset` will be executed once for each set of tags found. In an iteration, you can use the variable name and the tag name to reference the tags.  In the example above, the variable references to make would be `$str.a` and `$str.b`.
- Text files reference tags enclosed in double braces (e.g.`{{}}`).

Example text file for the above script:

```
{{a}}
Hello world!

{{b}}
Goodbye world!
```

#### `echo`: Write to stdout

`echo text`

### Loading and Saving Images

#### `load-image`: Load an image

`load-image path :`

- `load-image` begins a block wherein that image is the active image. There may be only one image active at a given time
- Any changes made to the image are discarded unless you save the image
- The only currently supported format for loading is **24-bit bitmaps (.bmp)**.

#### `save-image`: Save an image

`save-image path`

- Save supports both the 24-bit bitmap file format and the PNG format.

### Cut and Paste

#### `snip`: Copy an image to memory

`snip [transform] -> varName`

for example

```
snip -> x      # copy the selection as-is
snip -90 -> x2 # copy the selection, and rotate it clockwise 90 degrees
```

- You can unload images once copied, and the copy remains available
- When a rotate is indicated, the snippet size is enlarged to prevent clipping

#### `overlay`: Paste an image from memory

`overlay varName [pnt{1,2}] [rgb{0,0,0}]`

- `varName` must refer to a copy previously made with `snip`
- The point argument optionally defines the location inside the active selection to paste
- The color argument optionally defines a color in the copy to treat as transparent (the default is white)
- `overlay` will attempt to expand the active selection boundaries if the snippet is too big. If expansion is required but only a subset is the active selection (e.g. you've used `select-object`) crayon will error.

### Selection

### Text

### Adding and Removing Outlines

### Demarcation (i.e. Finding Image Landmarks)

### Pixel Transforms
