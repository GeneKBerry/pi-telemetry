
const canvas = document.getElementById("temperatureChart");
const ctx = canvas.getContext("2d");

const temperatureDisplay = document.getElementById("temperature");
const minimumDisplay = document.getElementById("minimum");
const maximumDisplay = document.getElementById("maximum");

const temperatures = [];
const MAX_READINGS = 60;

let simulatedTemperature = 42.0;

// Generate simulated readings until the Pi collector is connected
function getTemperature() {
    simulatedTemperature += (Math.random() - 0.5) * 1.2;
    simulatedTemperature = Math.max(
        38,
        Math.min(55, simulatedTemperature)
    );

    return simulatedTemperature;
}

// Smooth readings using a 3-point moving average
function smoothReadings(data) {
    return data.map((value, index) => {
        if (index === 0 || index === data.length - 1) {
            return value;
        }

        return (
            data[index - 1] +
            value +
            data[index + 1]
        ) / 3;
    });
}

// Draw the temperature graph
function drawGraph() {
    const width = canvas.clientWidth;
    const height = canvas.clientHeight;
    const pixelRatio = window.devicePixelRatio || 1;

    canvas.width = Math.round(width * pixelRatio);
    canvas.height = Math.round(height * pixelRatio);

    ctx.setTransform(pixelRatio, 0, 0, pixelRatio, 0, 0);
    ctx.clearRect(0, 0, width, height);

    const left = 50;
    const right = width - 20;
    const top = 20;
    const bottom = height - 35;

    const graphWidth = right - left;
    const graphHeight = bottom - top;

    if (temperatures.length === 0) {
        return;
    }

    const smooth = smoothReadings(temperatures);

    // Dynamic vertical scaling with a minimum 4-degree range
    const lowest = Math.min(...smooth);
    const highest = Math.max(...smooth);

    const center = (lowest + highest) / 2;
    const range = Math.max(4, highest - lowest + 2);

    const minY = center - range / 2;
    const maxY = center + range / 2;

    // Draw grid and temperature labels
    ctx.strokeStyle = "#334155";
    ctx.fillStyle = "#94a3b8";
    ctx.font = "12px Arial";
    ctx.lineWidth = 1;

    for (let i = 0; i <= 5; i++) {
        const y = top + (i / 5) * graphHeight;
        const temperature = maxY - (i / 5) * range;

        ctx.beginPath();
        ctx.moveTo(left, y);
        ctx.lineTo(right, y);
        ctx.stroke();

        ctx.fillText(temperature.toFixed(1), 5, y + 4);
    }

    // Draw smoothed temperature line
    ctx.strokeStyle = "#38bdf8";
    ctx.lineWidth = 3;
    ctx.lineJoin = "round";
    ctx.lineCap = "round";

    ctx.beginPath();

    smooth.forEach((temperature, index) => {
        const x = temperatures.length === 1
            ? left
            : left + (index / (temperatures.length - 1)) * graphWidth;

        const y = bottom -
            ((temperature - minY) / range) * graphHeight;

        if (index === 0) {
            ctx.moveTo(x, y);
        } else {
            ctx.lineTo(x, y);
        }
    });

    ctx.stroke();
}

// Update dashboard once per second
function updateDashboard() {
    const temperature = getTemperature();

    temperatures.push(temperature);

    if (temperatures.length > MAX_READINGS) {
        temperatures.shift();
    }

    temperatureDisplay.textContent = temperature.toFixed(2);
    minimumDisplay.textContent =
        Math.min(...temperatures).toFixed(2);
    maximumDisplay.textContent =
        Math.max(...temperatures).toFixed(2);

    drawGraph();
}

window.addEventListener("resize", drawGraph);

updateDashboard();
setInterval(updateDashboard, 1000);
