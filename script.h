const char script_js[] PROGMEM = R"rawliteral(

function updateParking()
{
    fetch('/data')
    .then(response => response.json())
    .then(data => {

        document.getElementById("slot1").innerHTML = data.slot1;
        document.getElementById("slot2").innerHTML = data.slot2;
        document.getElementById("slot3").innerHTML = data.slot3;
        document.getElementById("slot4").innerHTML = data.slot4;

        document.getElementById("available").innerHTML = data.available;
        document.getElementById("recommend").innerHTML = data.recommend;

        setColor("slot1", data.slot1);
        setColor("slot2", data.slot2);
        setColor("slot3", data.slot3);
        setColor("slot4", data.slot4);

    })
    .catch(error => console.log(error));
}

function setColor(id, value)
{
    if(value == "Empty")
    {
        document.getElementById(id).style.color = "green";
        document.getElementById(id).style.fontWeight = "bold";
    }
    else
    {
        document.getElementById(id).style.color = "red";
        document.getElementById(id).style.fontWeight = "bold";
    }
}

updateParking();

setInterval(updateParking,1000);

)rawliteral";