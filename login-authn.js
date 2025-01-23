var loginscreen = document.getElementById("login-screen");
loginscreen.innerHTML = `
login-system-fail
`;
loginscreen.style.position = "absolute";
loginscreen.style.width = "100vw";
loginscreen.style.height = "100vh";
loginscreen.style.top="0px";
loginscreen.style.left="0px";
loginscreen.style.bottom="0px";
loginscreen.style.right="0px";
document.body.style.backgroundImage = "url('background.png')"; // Corrected syntax
loginscreen.style.backgroundImage = "url('background.png')"; // Corrected syntax
loginscreen.style.backgroundSize = "cover"; // Optional: Makes the image cover the entire area
loginscreen.style.backgroundRepeat = "no-repeat"; // Optional: Prevents the image from repeating
loginscreen.style.backgroundPosition = "center"; // Optional: Centers the image
