---
nav_order: 11
title: Website Configuration
---

# Locally editing the website

#### We will remove this page in the future, but it is useful if anyone else wants to update the website.

- Ensure you have `Jekyll`and its pre-requisites (mainly `Ruby` and `RubyGems`) installed. See [here](https://jekyllrb.com/docs/).
- Navigate to the `docs/` directory
- Execute the following in your command line:

```
bundle exec jekyll serve --livereload
```

After a few seconds this should give you an output as follows:

```
...
LiveReload address: http://127.0.0.1:35729
Server address: http://127.0.0.1:4000
Server running... press ctrl-c to stop.
```

- Navigate to the server address specified
- Make changes as required and the webpage will automatically be updated. See [here](https://pmarsceill.github.io/just-the-docs/)
  for customisation options for the theme we are using. (Feel free to change the theme, I just thought this one works well for
  simplicity and how easy it is to add content)
- Note - if you modify the `_config.yml` file you will need to restart the server by executing step 3

# Locally generating Doxygen Documentation

- Ensure you have `doxygen` installed
- From the root of the repository execute the following:

```
doxygen docs/Doxyfile
```

- You can then run the website locally as explained above with the updated documentation to verify that Doxygen has generated the docs correctly
- Note that this will generate lots of html files in `docs/html` however these **do not** need to be pushed to GitHub as the action will generate this automatically
