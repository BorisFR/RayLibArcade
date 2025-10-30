function includeHTML() {
    var z, i, elmnt, file, xhttp;
    /*loop through a collection of all HTML elements:*/
    z = document.getElementsByTagName("*");
    for (i = 0; i < z.length; i++) {
        elmnt = z[i];
        /*search for elements with a certain atrribute:*/
        file = elmnt.getAttribute("w3-include-html");
        if (file) {
            /*make an HTTP request using the attribute value as the file name:*/
            xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4) {
                    if (this.status == 200) { elmnt.innerHTML = this.responseText; }
                    if (this.status == 404) { elmnt.innerHTML = "Page not found."; }
                    /*remove the attribute, and call this function once more:*/
                    elmnt.removeAttribute("w3-include-html");
                    includeHTML();
                }
            }
            xhttp.open("GET", file, true);
            xhttp.send();
            /*exit the function:*/
            return;
        }
    }
};

let gamesInfo = new Map();
function populatePopup(game) {
    let info = gamesInfo.get(game);
    document.getElementById("gameTitle").innerHTML = "<img src=\"../sdcard/_icon/" + game + ".png\" alt=\"" + info["name"] + "\" /> " + info["name"];
    document.getElementById("gameScreen").src = "../sdcard/ss/" + game + ".png";
    document.getElementById('romName').textContent = game;
    document.getElementById('machineType').textContent = info["machine"];
    const tableBody = document.getElementById("romList").getElementsByTagName("tbody")[0];
    while (tableBody.firstChild) { tableBody.removeChild(tableBody.firstChild); }
    info["rom"].forEach(rom => {
        let newRow = tableBody.insertRow();
        newRow.classList.add("tableBorder1");
        let nameCell = newRow.insertCell(0);
        nameCell.classList.add("tableBorder1");
        nameCell.textContent = rom["name"];
        nameCell = newRow.insertCell(1);
        nameCell.classList.add("tableBorder1");
        nameCell.textContent = rom["length"];
        nameCell = newRow.insertCell(2);
        nameCell.classList.add("tableBorder1");
        nameCell.textContent = rom["crc"];
        nameCell = newRow.insertCell(3);
        nameCell.classList.add("tableBorder1");
        nameCell.textContent = rom["type"];
    });
};
function openPopup(game) {
    if (!gamesInfo.has(game)) {
        let name = game + ".json";
        fetch(name)
            .then(response => {
                if (!response.ok) { throw new Error('HTTP Error Status: ' + response.status); }
                return response.json();
            })
            .then(data => { gamesInfo.set(game, data); populatePopup(game); })
            .catch(error => {
                console.error('Error loading JSON:', error);
                document.getElementById('output').textContent = 'Failed to load JSON: ' + error;
            })
    } else { populatePopup(game); }
    document.getElementById("overlay").style.display = "block";
};
function closePopup() { document.getElementById("overlay").style.display = "none"; }
window.onclick = function (event) {
    const overlay = document.getElementById("overlay");
    if (event.target === overlay) { closePopup(); }
};
document.addEventListener('keydown', function (event) {
    if (event.key === 'Escape') { closePopup(); }
    else if (event.keyCode === 27) { closePopup(); }
});
function createLink(element) {
    var x = "<div onclick=\"openPopup('" + element["id"] + "')\"><img src=\"../sdcard/_icon/" + element["id"] + ".png\" alt=\"" + element["name"] + "\" /> <span class=\"gameTitle\">" + element["name"] + "</span></div>";
    return x;
};
function dolist() {
    console.log("loading");
    fetch('./wiki/gamelist.json')
        .then(response => {
            if (!response.ok) { throw new Error('HTTP Error Status: ' + response.status); }
            return response.json();
        })
        .then(data => {
            document.getElementById('output').innerHTML = "";
            data.games.sort((a, b) => {
                if (a["name"] < b["name"]) return -1;
                return 1;
            });
            data.games.forEach(element => {
                document.getElementById('output').innerHTML = document.getElementById('output').innerHTML + createLink(element);
            });
        })
        .catch(error => {
            console.error('Error loading JSON:', error);
            document.getElementById('output').textContent = 'Failed to load JSON: ' + error;
        })
};