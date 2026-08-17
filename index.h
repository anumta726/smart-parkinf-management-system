const char index_html[] PROGMEM = R"rawliteral(

<!DOCTYPE html>
<html>

<head>

<meta charset="UTF-8">

<meta name="viewport" content="width=device-width, initial-scale=1.0">

<title>Smart Parking</title>

<link rel="stylesheet" href="/style.css">

</head>

<body>

<div class="container">

<h1>🚗 Smart Parking System</h1>

<table>

<tr>
<th>Parking Slot</th>
<th>Status</th>
</tr>

<tr>
<td>Slot 1</td>
<td id="slot1">Loading...</td>
</tr>

<tr>
<td>Slot 2</td>
<td id="slot2">Loading...</td>
</tr>

<tr>
<td>Slot 3</td>
<td id="slot3">Loading...</td>
</tr>

<tr>
<td>Slot 4</td>
<td id="slot4">Loading...</td>
</tr>

</table>

<div class="box">

<h3>Available Slots :
<span id="available">0</span>
</h3>

<h3>Recommended Slot :
<span id="recommend">--</span>
</h3>

</div>

<p class="online">
Status : ONLINE
</p>

</div>

<script src="/script.js"></script>

</body>

</html>

)rawliteral";