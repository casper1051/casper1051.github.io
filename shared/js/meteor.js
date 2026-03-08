
const canvas_978125 = document.createElement("canvas");
canvas_978125.id = "meteorCanvas";

document.body.prepend(canvas_978125);

const canvas = canvas_978125;
const ctx = canvas.getContext('2d');

let width, height;
let stars = [];

function init() {
  width = window.innerWidth;
  height = window.innerHeight;
  canvas.width = width;
  canvas.height = height;
}

class Meteor {
  constructor() {
    this.reset();
  }

  reset() {
    this.x = Math.random() * width + (width * 0.2); 
    this.y = Math.random() * -height;
    this.size = Math.random() * 2 + 1;
    this.speed = Math.random() * 10 + 2;
    this.length = Math.random() * 150 + 50;
  }

  update() {
    this.x -= this.speed;
    this.y += this.speed;
    if (this.y > height || this.x < -this.length) {
      this.reset();
    }
  }

  draw() {
    const gradient = ctx.createLinearGradient(
        this.x, this.y, 
        this.x + this.length, this.y - this.length
    );
    gradient.addColorStop(0, 'rgba(255, 255, 255, 1)'); 
    gradient.addColorStop(0.1, 'rgba(95, 145, 255, 0.8)'); 
    gradient.addColorStop(1, 'rgba(0, 0, 0, 0)'); 

    ctx.strokeStyle = gradient;
    ctx.lineWidth = this.size;
    ctx.lineCap = 'round';
    
    ctx.beginPath();
    ctx.moveTo(this.x, this.y);
    ctx.lineTo(this.x + this.length, this.y - this.length);
    ctx.stroke();
  }
}


function createMeteors(count) {
  for (let i = 0; i < count; i++) {
    stars.push(new Meteor());
  }
}

function animate() {
  ctx.clearRect(0, 0, width, height); 
  
  stars.forEach(star => {
    star.update();
    star.draw();
  });
  
  requestAnimationFrame(animate);
}

window.addEventListener('resize', init);


init();
createMeteors(30); 
animate();