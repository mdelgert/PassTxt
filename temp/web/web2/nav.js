//console.log('Hello!');

// Function to load the navigation bar from nav.html
document.addEventListener('DOMContentLoaded', () => {
  fetch('nav.html')
    .then(response => response.text())
    .then(data => {
      document.getElementById('nav-placeholder').innerHTML = data;
      const links = document.querySelectorAll('nav a');
      links.forEach(link => {
        if (link.href === window.location.href) {
          link.setAttribute('aria-current', 'page');
        } else {
          link.removeAttribute('aria-current');
        }
      });
    });
});
