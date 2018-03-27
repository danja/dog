I'm using the Atom IDE to edit these docs in GitHub-flavoured Markdown.

As well as HTML, I wanted a PDF rendering, but the converter built into Atom fails (HTML preview works fine).

Gimli works a treat from the command line.

Installation needs (at least) ruby, ruby-dev, rubygems and wkhtmltopdf.

Command:

$gimli -f ./README.md
