
<center>
  <img src="https://raw.githubusercontent.com/zachary62/Dynamic-f-tree/gh-pages/files/images/back_arch.png" alt="hi" class="inline" width=700px />
</center>

## Introduction

Traditional database cleaning and explanation systems reply primarily on the user’s complaint as the quality measure and make strong assumptions about the error types. For systems which generate explanations by perturbing input values through models, they either use very simple models or assume that user can provide magical models. This is a pain point: data scientists may have domain-specific knowledge, but asking them to provide models before using the system is impractical. 

Reptile is an innovative explanation system that guides user as they incrementally make complaint drill down into the hierarchies. Reptile leverages domain-specific knowledge and hierarchical structure to repair dataset at aggregation level and recommend next drill down. 

At the minimum, user only needs to provide the dataset and aggregation functions they want to inspect, and Reptile is all set! Reptile will work like an outlier detector to resolve user's complaints. Reptile allows users to provide small pieces of data as domain-specific knowledge, and Reptile will combine these signals to make more meaningful explanations.




<!---
## Welcome to GitHub Pages

You can use the [editor on GitHub](https://github.com/zachary62/Dynamic-f-tree/edit/gh-pages/index.md) to maintain and preview the content for your website in Markdown files.

Whenever you commit to this repository, GitHub Pages will run [Jekyll](https://jekyllrb.com/) to rebuild the pages in your site, from the content in your Markdown files.

### Markdown

Markdown is a lightweight and easy-to-use syntax for styling your writing. It includes conventions for

```markdown
Syntax highlighted code block

# Header 1
## Header 2
### Header 3

- Bulleted
- List

1. Numbered
2. List

**Bold** and _Italic_ and `Code` text

[Link](url) and ![Image](src)
```

For more details see [GitHub Flavored Markdown](https://guides.github.com/features/mastering-markdown/).

### Jekyll Themes

Your Pages site will use the layout and styles from the Jekyll theme you have selected in your [repository settings](https://github.com/zachary62/Dynamic-f-tree/settings). The name of this theme is saved in the Jekyll `_config.yml` configuration file.

### Support or Contact

Having trouble with Pages? Check out our [documentation](https://docs.github.com/categories/github-pages-basics/) or [contact support](https://github.com/contact) and we’ll help you sort it out.
---!>
