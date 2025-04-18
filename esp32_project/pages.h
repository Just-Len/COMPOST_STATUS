const char *index_page = R"_(
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
    .tooltip { position: absolute; background-color: rgba(0, 0, 0, 0.7); color: white; padding: 8px; border-radius: 4px; pointer-events: none; display: none; }
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
      <button onclick="updateChart('Humedad')">Humedad</button>
      <button onclick="updateChart('Acidez')">Acidez</button>
      <button onclick="updateChart('Temperatura')">Temperatura</button>
    </div>
    <h3>Últimas Mediciones</h3>
    <p>Humedad: <span id="humidityValue">Cargando medicion</span></p>
    <p>Acidez (pH): <span id="phValue">Cargando medicion</span></p>
    <p>Temperatura: <span id="temperatureValue">Cargando medicion</span></p>
    <button type="button" onclick="loadSensorData()">Cargar Datos del Sensor</button>

    <div class="form-container">
      <h3>Agregar Nueva Medicción</h3>
      <form id="addForm">
        <label>Fecha (DD-MM-YYYY): <input type="text" id="newFecha"></label><br><br>
        <label>Humedad: <input type="number" id="newHumedad"></label><br><br>
        <label>Acidez: <input type="number" id="newAcidez"></label><br><br>
        <label>Temperatura: <input type="number" id="newTemperatura"></label><br><br>
        <label>Contenedor: <input type="number" id="newContenedor"></label><br><br>
        <button type="button" onclick="addNewMeasurement()">Agregar</button>
      </form>
    </div>
    <br>
    <button onclick="downloadCSV()">Descargar CSV</button>
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
  let csvData=[],tooltip=document.getElementById("tooltip");function handleFile(e){var e=e.target.files[0],t=new FileReader;t.onload=function(e){e=e.target.result.split("\n").filter(e=>""!==e.trim()),e=(csvData=e.slice(1).map(e=>e.split(","))).filter(e=>5!==e.length||e.some(e=>""===e.trim()));if(0<e.length){if(!confirm(`El archivo contiene ${e.length} filas con datos faltantes. ¿Omitir filas erróneas?`))return;csvData=csvData.filter(e=>5===e.length&&e.every(e=>""!==e.trim()))}displayData(csvData)},t.readAsText(e)}function displayData(e){let t=document.getElementById("dataBody");t.innerHTML="",e.forEach(e=>{let n=document.createElement("tr");e.forEach(e=>{var t=document.createElement("td");t.textContent=e.trim(),n.appendChild(t)}),t.appendChild(n)})}function updateChart(p){if(0!==csvData.length){let t={Humedad:1,Acidez:2,Temperatura:3,Contenedor:4}[p],l=csvData.map(e=>e[0]);var e=csvData.map(e=>parseFloat(e[t]));let i=csvData.map(e=>e[4]),d=document.getElementById("lineChart");d.innerHTML="";var n=Math.max(...e);let s=Math.min(...e),c=(d.width.baseVal.value-100)/(e.length-1),u=(d.height.baseVal.value-100)/(n-s);n=document.createElementNS("http://www.w3.org/2000/svg","line"),n=(n.setAttribute("x1",50),n.setAttribute("y1",250),n.setAttribute("x2",550),n.setAttribute("y2",250),n.setAttribute("stroke","black"),d.appendChild(n),document.createElementNS("http://www.w3.org/2000/svg","line")),n=(n.setAttribute("x1",50),n.setAttribute("y1",50),n.setAttribute("x2",50),n.setAttribute("y2",250),n.setAttribute("stroke","black"),d.appendChild(n),l.forEach((e,t)=>{var t=50+t*c,n=document.createElementNS("http://www.w3.org/2000/svg","text");n.setAttribute("x",t),n.setAttribute("y",270),n.setAttribute("font-size","10px"),n.setAttribute("text-anchor","middle"),n.textContent=e,d.appendChild(n)}),document.createElementNS("http://www.w3.org/2000/svg","path"));let m="";e.forEach((t,n)=>{var e=50+n*c,a=250-(t-s)*u,r=(0===n?m=`M${e},`+a:m+=` L${e},`+a,document.createElementNS("http://www.w3.org/2000/svg","circle")),o=(r.setAttribute("cx",e),r.setAttribute("cy",a),r.setAttribute("r",4),r.setAttribute("fill","#83072D"),d.appendChild(r),document.createElementNS("http://www.w3.org/2000/svg","text"));o.setAttribute("x",e),o.setAttribute("y",a-10),o.setAttribute("font-size","10px"),o.setAttribute("text-anchor","middle"),o.textContent=t.toFixed(2),d.appendChild(o),r.addEventListener("mouseover",function(e){tooltip.style.display="block",tooltip.textContent=`${p}: ${t.toFixed(2)} (Fecha: ${l[n]}) - Contenedor: `+i[n],tooltip.style.left=e.pageX+10+"px",tooltip.style.top=e.pageY-30+"px"}),r.addEventListener("mouseout",function(){tooltip.style.display="none"})}),n.setAttribute("d",m),n.setAttribute("stroke","#83072D"),n.setAttribute("fill","none"),d.appendChild(n)}}function addNewMeasurement(){var e=elementValue("newFecha"),t=elementValue("newHumedad"),n=elementValue("newAcidez"),a=elementValue("newTemperatura"),r=elementValue("newContenedor");e&&t&&n&&a&&r?(e=[e,t+"%",n+"%",a,r],csvData.push(e),displayData(csvData),alert("Medición agregada correctamente.")):alert("Por favor complete todos los campos. O cheque que esten validos")}function downloadCSV(){var e="Fecha,Humedad,Acidez,Temperatura,Contenedor\n"+csvData.map(e=>e.join(",")).join("\n"),e=new Blob([e],{type:"text/csv;charset=utf-8;"}),t=document.createElement("a");void 0!==t.download&&(t.href=URL.createObjectURL(e),t.download="datos_compostera.csv",t.click())}function getMeasurements(){fetch("/measurements").then(e=>e.json()).then(e=>{document.getElementById("humidityValue").textContent=e.humidity+"%",document.getElementById("phValue").textContent=e.pH,document.getElementById("temperatureValue").textContent=e.temperature+"°C"}).catch(e=>console.error("Error fetching data:",e))}function loadSensorData(){fetch("/measurements").then(e=>e.json()).then(e=>{document.getElementById("newFecha").value=(new Date).toLocaleDateString("es-ES"),document.getElementById("newHumedad").value=e.humidity||"",document.getElementById("newAcidez").value=e.pH||"",document.getElementById("newTemperatura").value=e.temperature||"",document.getElementById("newContenedor").value=""}).catch(e=>{alert("Error al cargar los datos del sensor."),console.error("Error fetching data:",e)})}function elementValue(e){return document.getElementById(e).value}document.getElementById("fileInput").addEventListener("change",handleFile,!1),setInterval(getMeasurements,5e3),window.onload=getMeasurements;
  </script>
</body>
</html>
)_";