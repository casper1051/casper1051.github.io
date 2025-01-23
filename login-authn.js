const loginscreen = document.getElementById("login-screen");

// Define the login screen HTML structure
loginscreen.innerHTML = `
  <div class="login-container">
    <h1 class="login-title">Welcome</h1>
    <form class="login-form">
      <input type="text" class="login-input" id="username" placeholder="Username" required />
      <input type="password" class="login-input" id="password" placeholder="Password" required />
      <button type="submit" class="login-button">Login</button>
    </form>
    <p class="login-footer">Forgot your password? <a href="#">womp womp</a>.</p>
  </div>
`;

// Add styles and animations to the login screen
const style = document.createElement("style");
style.innerHTML = `
  body {
    margin: 0;
    padding: 0;
    display: flex;
    justify-content: center;
    align-items: center;
    height: 100vh;
    background-image: url('background.png');
    background-size: cover;
    background-position: center;
    font-family: 'monospace', sans-serif;
    color: white;
    overflow: hidden;
  }

  .login-container {
    text-align: center;
    background: rgba(0, 0, 0, 0.8);
    padding: 30px;
    border-radius: 10px;
    box-shadow: 0 8px 20px rgba(0, 0, 0, 0.5);
    animation: fadeIn 1s ease-in-out;
  }

  .login-title {
    margin: 0 0 20px;
    font-size: 2rem;
  }

  .login-form {
    display: flex;
    flex-direction: column;
  }

  .login-input {
    margin-bottom: 15px;
    padding: 10px;
    font-size: 1rem;
    border: none;
    border-radius: 5px;
    outline: none;
    background: #333;
    color: white;
  }

  .login-input:focus {
    background: #444;
    outline: 2px solid #555;
  }

  .login-button {
    padding: 10px;
    font-size: 1rem;
    border: none;
    border-radius: 5px;
    background-color: #007bff;
    color: white;
    cursor: pointer;
    transition: background-color 0.3s ease;
  }

  .login-button:hover {
    background-color: #0056b3;
  }

  .login-footer {
    margin-top: 10px;
    font-size: 0.9rem;
  }

  .login-footer a {
    color: #007bff;
    text-decoration: none;
  }

  .login-footer a:hover {
    text-decoration: underline;
  }

  @keyframes fadeIn {
    from {
      opacity: 0;
      transform: translateY(-20px);
    }
    to {
      opacity: 1;
      transform: translateY(0);
    }
  }
`;

// Append the style to the document head
document.head.appendChild(style);

// Add functionality for the login button
const loginForm = document.querySelector(".login-form");
loginForm.addEventListener("submit", (e) => {
  e.preventDefault(); // Prevent page reload
  const username = document.getElementById("username").value;
  const password = document.getElementById("password").value;

  // Mock authentication (replace with real authentication logic)
  if (username === "admin" && password === "password") {
    alert("Login successful!");
  } else {
    alert("Invalid username or password.");
  }
});
