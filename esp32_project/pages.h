const char *index_page = R"(
<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8">
  <title>Sensores Compostera</title>
  <style>
    body { font-family:'Franklin Gothic Medium', 'Arial Narrow', Arial, sans-serif; background-color:  aliceblue; display: flex; flex-direction: row; gap: 20px; }
    .styled_table { border-collapse: collapse; margin: 25px 0; font-size: 0.9em; font-family: sans-serif; min-width: 400px; }
    .styled_table thead tr { background-color: #83072D; color: #ffffff; }
    .styled_table th, .styled_table td { padding: 12px 15px; }
    .buttons { display: flex; gap: 10px; margin-bottom: 15px; }
    .tooltip { position: absolute; background-color: rgba(0, 0, 0, 0.7);
       color: white; padding: 8px; border-radius: 4px; pointer-events: none; display: none; }
      button{padding: 10px 20px;background-color: #83072D;color: white;border: none;border-radius: 4px;cursor: pointer;}
      button:hover {background-color: #661c2b;}
    .form-container { margin-top: 20px; }
  </style>
</head>
<body>
  <div>
    <h2>Estadísticas Compostera</h2>
    <input type="file" id="fileInput" accept=".csv">
    <h3>Graficas</h3>
    <div class="buttons">
      <button onclick=\"updateChart('Humedad')\">Humedad</button>
      <button onclick=\"updateChart('Acidez')\">Acidez</button>
      <button onclick=\"updateChart('Temperatura')\">Temperatura</button>
    </div>
    <h3>Últimas Mediciones</h3>
    <p>Humedad: <span id="humidityValue">Cargando medicion</span></p>
    <p>Acidez (pH): <span id="phValue">Cargando medicion</span></p>
    <p>Temperatura: <span id="temperatureValue">Cargando medicion</span></p>
    <button type="button" onclick=\"loadSensorData()\">Cargar Datos del Sensor</button>

    <div class="form-container">
      <h3>Agregar Nueva Medicción</h3>
      <form id="addForm">
        <label>Fecha (DD-MM-YYYY): <input type="text" id="newFecha"></label><br><br>
        <label>Humedad: <input type="number" id="newHumedad"></label><br><br>
        <label>Acidez: <input type="number" id="newAcidez"></label><br><br>
        <label>Temperatura: <input type="number" id="newTemperatura"></label><br><br>
        <label>Contenedor: <input type="number" id="newContenedor"></label><br><br>
        <button type="button" onclick=\"addNewMeasurement()\">Agregar</button>
      </form>
    </div>
    <br>
    <button onclick=\"downloadCSV()\">Descargar CSV</button>
  </div>
  <div class="table-container">
    <table class="styled_table" style="margin-top: 148PX;">
      <thead>
        <tr style="border-radius: 40px;"><th>Fecha</th><th>Humedad</th><th>Acidez</th><th>Temperatura</th><th>Contenedor</th></tr>
      </thead>
      <tbody id="dataBody"></tbody>
    </table>
  </div>
  <div style="margin-top: 148px;"><svg id="lineChart" width="600" height="300"></svg></div>
  <div class="tooltip" id="tooltip"></div>
  <script>
    let csvData = [];
    const tooltip = document.getElementById('tooltip');
    document.getElementById('fileInput').addEventListener('change', handleFile, false);
    function handleFile(event) {
      const file = event.target.files[0];
      const reader = new FileReader();
      reader.onload = function(e) {
        const rows = e.target.result.split('\n').filter(row => row.trim() !== '');
        csvData = rows.slice(1).map(row => row.split(','));
        
        const invalidRows = csvData.filter(row => row.length !== 5 || row.some(cell => cell.trim() === ''));
        if (invalidRows.length > 0) {
          const proceed = confirm(`El archivo contiene ${invalidRows.length} filas con datos faltantes. ¿Omitir filas erróneas?`);
          if (!proceed) return;
          csvData = csvData.filter(row => row.length === 5 && row.every(cell => cell.trim() !== ''));
        }
        
        displayData(csvData);
      };
      reader.readAsText(file);
    }
    function displayData(data) {
      const tableBody = document.getElementById('dataBody');
      tableBody.innerHTML = '';
      data.forEach(row => {
        const tr = document.createElement('tr');
        row.forEach(cell => {
          const td = document.createElement('td');
          td.textContent = cell.trim();
          tr.appendChild(td);
        });
        tableBody.appendChild(tr);
      });
    }
    function updateChart(column) {
      if (csvData.length === 0) return;
      const columnMap = { 'Humedad': 1, 'Acidez': 2, 'Temperatura': 3, 'Contenedor': 4 };
      const columnIndex = columnMap[column];
      const labels = csvData.map(row => row[0]);
      const data = csvData.map(row => parseFloat(row[columnIndex]));
      const contenedorData = csvData.map(row => row[4]); 
      const svg = document.getElementById('lineChart');
      svg.innerHTML = '';
      const maxVal = Math.max(...data);
      const minVal = Math.min(...data);
      const scaleX = (svg.width.baseVal.value - 100) / (data.length - 1);
      const scaleY = (svg.height.baseVal.value - 100) / (maxVal - minVal);
      const xAxis = document.createElementNS("http://www.w3.org/2000/svg", "line");
      xAxis.setAttribute("x1", 50);
      xAxis.setAttribute("y1", 250);
      xAxis.setAttribute("x2", 550);
      xAxis.setAttribute("y2", 250);
      xAxis.setAttribute("stroke", "black");
      svg.appendChild(xAxis);
      const yAxis = document.createElementNS("http://www.w3.org/2000/svg", "line");
      yAxis.setAttribute("x1", 50);
      yAxis.setAttribute("y1", 50);
      yAxis.setAttribute("x2", 50);
      yAxis.setAttribute("y2", 250);
      yAxis.setAttribute("stroke", "black");
      svg.appendChild(yAxis);
      labels.forEach((label, i) => {
        const x = 50 + i * scaleX;
        const text = document.createElementNS("http://www.w3.org/2000/svg", "text");
        text.setAttribute("x", x);
        text.setAttribute("y", 270);
        text.setAttribute("font-size", "10px");
        text.setAttribute("text-anchor", "middle");
        text.textContent = label;
        svg.appendChild(text);
      });
      const path = document.createElementNS("http://www.w3.org/2000/svg", "path");
      let pathData = '';
      data.forEach((value, i) => {
        const x = 50 + i * scaleX;
        const y = 250 - (value - minVal) * scaleY;
        if (i === 0) {
          pathData = `M${x},${y}`;
        } else {
          pathData += ` L${x},${y}`;
        }
        const circle = document.createElementNS("http://www.w3.org/2000/svg", "circle");
        circle.setAttribute("cx", x);
        circle.setAttribute("cy", y);
        circle.setAttribute("r", 4);
        circle.setAttribute("fill", "#83072D");
        svg.appendChild(circle);
        const textData = document.createElementNS("http://www.w3.org/2000/svg", "text");
        textData.setAttribute("x", x);
        textData.setAttribute("y", y - 10); 
        textData.setAttribute("font-size", "10px");
        textData.setAttribute("text-anchor", "middle");
        textData.textContent = value.toFixed(2);
        svg.appendChild(textData);
        circle.addEventListener('mouseover', function(event) {
          tooltip.style.display = 'block';
          tooltip.textContent = `${column}: ${value.toFixed(2)} (Fecha: ${labels[i]}) - Contenedor: ${contenedorData[i]}`;
          tooltip.style.left = `${event.pageX + 10}px`;
          tooltip.style.top = `${event.pageY - 30}px`;
        });
        circle.addEventListener('mouseout', function() {
          tooltip.style.display = 'none';
        });
      });
      path.setAttribute("d", pathData);
      path.setAttribute("stroke", "#83072D");
      path.setAttribute("fill", "none");
      svg.appendChild(path);
    }
    function addNewMeasurement() {
      const fecha = document.getElementById('newFecha').value;
      let humedad = document.getElementById('newHumedad').value;
      const acidez = document.getElementById('newAcidez').value;
      const temperatura = document.getElementById('newTemperatura').value;
      const contenedor = document.getElementById('newContenedor').value;
      if (!fecha || !humedad || !acidez || !temperatura || !contenedor) {
        alert('Por favor complete todos los campos. O cheque que esten validos');
        return;
      }
      const newRow = [fecha, humedad+"%", acidez+"%", temperatura, contenedor];
      csvData.push(newRow);
      displayData(csvData);
      alert("Medición agregada correctamente.");
    }
    function downloadCSV() {
      const csvContent = "Fecha,Humedad,Acidez,Temperatura,Contenedor\n" + csvData.map(row => row.join(',')).join('\n');
      const blob = new Blob([csvContent], { type: 'text/csv;charset=utf-8;' });
      const link = document.createElement('a');
      if (link.download !== undefined) {
        link.href = URL.createObjectURL(blob);
        link.download = 'datos_compostera.csv';
        link.click();
      }
    }
    function getMeasurements() {
    fetch('/measurements')
      .then(response => response.json())
      .then(data => {
        document.getElementById('humidityValue').textContent = `${data.humidity}%`;
        document.getElementById('phValue').textContent = data.pH;
        document.getElementById('temperatureValue').textContent = `${data.temperature}°C`;
      })
      .catch(error => console.error('Error fetching data:', error));
    }
    function loadSensorData() {
  fetch('/measurements')
    .then(response => response.json())
    .then(data => {
      document.getElementById('newFecha').value = new Date().toLocaleDateString('es-ES');
      document.getElementById('newHumedad').value = data.humidity || '';
      document.getElementById('newAcidez').value = data.pH || '';
      document.getElementById('newTemperatura').value = data.temperature || '';
      document.getElementById('newContenedor').value = ''; 
    })
    .catch(error => {
      alert('Error al cargar los datos del sensor.');
      console.error('Error fetching data:', error);
    });
}

    setInterval(getMeasurements, 5000);
    window.onload = getMeasurements;
  </script>
</body>
</html>
)";