class Symbol extends HTMLElement {
  connectedCallback() {
    this.innerHTML = `<strong><div class="symbol-inner">${this.getAttribute('icon') || '?'}</div></strong>`;
  }
}
customElements.define('ui-symbol', Symbol);