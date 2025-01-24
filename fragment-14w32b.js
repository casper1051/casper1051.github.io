<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>MacOS-Style Desktop</title>
    <style>
        body {
            margin: 0;
            font-family: Arial, sans-serif;
            background: url('background.png') no-repeat center center fixed;
            background-size: cover;
            color: white;
            overflow: hidden;
        }
        #dock {
            position: fixed;
            bottom: 20px;
            left: 50%;
            transform: translateX(-50%);
            display: flex;
            justify-content: center;
            background: rgba(0, 0, 0, 0.7);
            border-radius: 15px;
            padding: 10px 20px;
        }
        .app {
            text-align: center;
            margin: 0 10px;
        }
        .app img {
            width: 50px;
            height: 50px;
            border-radius: 10px;
            box-shadow: 0px 0px 10px rgba(0, 255, 0, 0.7);
            transition: transform 0.2s, box-shadow 0.2s;
        }
        .app img:hover {
            transform: scale(1.2);
            box-shadow: 0px 0px 15px rgba(0, 255, 255, 0.8);
        }
        .app-label {
            margin-top: 5px;
            font-size: 12px;
        }
        .window {
            position: absolute;
            width: 600px;
            height: 400px;
            background: rgba(0, 0, 0, 0.9);
            color: white;
            border-radius: 8px;
            box-shadow: 0 5px 15px rgba(0, 0, 0, 0.5);
            overflow: hidden;
            display: flex;
            flex-direction: column;
        }
        .top-bar {
            background: rgba(50, 50, 50, 1);
            padding: 10px;
            cursor: grab;
            display: flex;
            justify-content: space-between;
            align-items: center;
        }
        .top-bar button {
            background: none;
            border: none;
            color: white;
            margin-left: 10px;
            cursor: pointer;
        }
        .top-bar button:hover {
            color: red;
        }
        .window-content {
            flex-grow: 1;
            overflow: auto;
            background: black;
        }
        iframe {
            width: 100%;
            height: 100%;
            border: none;
        }
    </style>
</head>
<body>
    <div id="dock"></div>

    <script>
        // JSON array of apps
        const apps = [
            {
                name: "Embedded App",
                icon: "embedded.png",
                type: "embedded",
                content: "https://www.example.com"
            },
            {
                name: "Bundled App",
                icon: "bundled.png",
                type: "bundled",
                content: "<h1>Hello, this is a bundled app!</h1><p>You can include any HTML content here.</p>"
            }
        ];

        const dock = document.getElementById("dock");

        // Create app icons in the dock
        apps.forEach(app => {
            const appDiv = document.createElement("div");
            appDiv.classList.add("app");

            const img = document.createElement("img");
            img.src = app.icon;
            img.alt = app.name;
            img.onclick = () => openApp(app);

            const label = document.createElement("div");
            label.classList.add("app-label");
            label.textContent = app.name;

            appDiv.appendChild(img);
            appDiv.appendChild(label);
            dock.appendChild(appDiv);
        });

        // Open app function
        function openApp(app) {
            const windowDiv = document.createElement("div");
            windowDiv.classList.add("window");

            const topBar = document.createElement("div");
            topBar.classList.add("top-bar");

            // Add close button
            const closeButton = document.createElement("button");
            closeButton.textContent = "✕";
            closeButton.onclick = () => windowDiv.remove();
            topBar.appendChild(closeButton);

            // Add fullscreen button
            const fullscreenButton = document.createElement("button");
            fullscreenButton.textContent = "⬜";
            fullscreenButton.onclick = () => {
                windowDiv.style.width = "100vw";
                windowDiv.style.height = "100vh";
                windowDiv.style.top = "0";
                windowDiv.style.left = "0";
            };
            topBar.appendChild(fullscreenButton);

            // Enable dragging
            let isDragging = false, offsetX = 0, offsetY = 0;
            topBar.onmousedown = (e) => {
                isDragging = true;
                offsetX = e.offsetX;
                offsetY = e.offsetY;
                document.onmousemove = (e) => {
                    if (isDragging) {
                        windowDiv.style.top = `${e.clientY - offsetY}px`;
                        windowDiv.style.left = `${e.clientX - offsetX}px`;
                    }
                };
                document.onmouseup = () => {
                    isDragging = false;
                    document.onmousemove = null;
                };
            };

            const title = document.createElement("span");
            title.textContent = app.name;
            topBar.insertBefore(title, closeButton);

            const contentDiv = document.createElement("div");
            contentDiv.classList.add("window-content");

            if (app.type === "embedded") {
                const iframe = document.createElement("iframe");
                iframe.src = app.content;
                contentDiv.appendChild(iframe);
            } else if (app.type === "bundled") {
                contentDiv.innerHTML = app.content;
            }

            windowDiv.appendChild(topBar);
            windowDiv.appendChild(contentDiv);
            document.body.appendChild(windowDiv);

            // Position window in the center
            windowDiv.style.top = `${window.innerHeight / 2 - 200}px`;
            windowDiv.style.left = `${window.innerWidth / 2 - 300}px`;
        }
    </script>
</body>
</html>
