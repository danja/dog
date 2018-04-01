I'm using the Atom IDE to edit these docs in [GitHub-flavoured Markdown](https://github.github.com/gfm/).

I'm using tables, as defined in GitHub-flavoured markdown, which the standard Linux 'markdown' app doesn't support. But 'marked' does.

sudo npm install -g marked

marked -i manual.md -o manual.html

As well as HTML, I wanted a PDF rendering, but the converter built into Atom fails (HTML preview works fine).

Gimli works a treat from the command line.

Installation needs (at least) ruby, ruby-dev, rubygems and wkhtmltopdf.

Command:

gimli -f manual.md
