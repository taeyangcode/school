// converting key terms to markdown format
const key_terms_to_markdown = () => {
  const key_terms_list = [
    ...document.getElementsByClassName("os-glossary-container")[0].children,
  ];
  return key_terms_list.reduce((accumulator, key_term) => {
    const [term, definition] = key_term.innerText.split("\n");
    return accumulator + `**${term}** - ${definition}\n\n`;
  }, "");
};
