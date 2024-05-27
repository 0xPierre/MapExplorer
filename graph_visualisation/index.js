var express = require('express')
var app = express();
const server_port = 3000

app.use(express.static(__dirname + '/public'));
app.listen(server_port, () => console.log(`App listening on port ${server_port}!`))

const fs = require('fs');
const readline = require('readline');

const filePath = '../TP-2A-S4/output/nodes.graph';
let index = 0;
let graphSize;
let map = [];
let newMap = {nodes: [], links: []};

// Create a readline interface
let rl = readline.createInterface({
    input: fs.createReadStream(filePath),
    crlfDelay: Infinity // To treat \r\n as a single newline
});

const fileChangedHandler = () => {
    console.log('File has changed. Re-reading file and executing function...');

    // Clear the map or do any other necessary cleanup
    map = [];

    newMap = {nodes: [], links: []};

    // Reinitialize index
    index = 0;

    // Read the file again and process its content
    rl.close(); // Close the existing readline interface

    // Create a new readline interface to read the file again
    const newRl = readline.createInterface({
        input: fs.createReadStream(filePath),
        crlfDelay: Infinity
    });

    newRl.on('line', (line) => {
        if (index > 0) {
            if (line.length > 0) {
                const test = line.split(' ').map(Number);
                map.push(test);
            } else {
                map.push([-1]);
            }
        }
        index++;
    });

    newRl.on('close', () => {
        console.log('File reading complete.');
        console.log('GRAPH SIZE: ' + graphSize);

        map.forEach(x => {
            console.log(x);
        })

        console.log('\n')

        for (let i = 0; i < map.length; i ++) {
            // console.log('ID ' + map[i][0]);
            let nodeId = map[i][0];
            let arcCount = map[i][1];

            let arc = [];

            let link = {};


            for (let j = 1; j < arcCount + 1; j++) {
                arc.push(map[i + j])

                link = {
                    source: String(nodeId),
                    target: String(map[i + j][0]),
                    label: {
                        show: true,
                        formatter: String(map[i + j][1])
                    }
                }
            }

            console.log('Id ' + nodeId + ' arcount ' + arcCount +' arc: ', arc)

            const node = {
                id: nodeId,
                name: String (nodeId)
                // arcCount: arcCount,
                // target: arc
            }

            newMap.nodes.push(node);
            if (Object.keys(link).length > 0)
                newMap.links.push(link)

            i += arcCount + 1
        }

        console.log(newMap)
        console.log(JSON.stringify(newMap))

        setInterval(() => {
            if (ws != null && Object.keys(newMap).length > 0) {
                ws.send(JSON.stringify(newMap)); // Sending data as a JSON string
            }
        }, 2000);

    })

    // Set the newly created readline interface to the global variable rl
    rl = newRl;
};

fs.watch(filePath, (eventType, filename) => {
    if (eventType === 'change') {
        fileChangedHandler();
    }
});

fileChangedHandler();


/**************************************************************************
 Websocket server that communicates with browser
 **************************************************************************/

const WebSocket = require('ws');

const wss = new WebSocket.Server({port: 8080});
var ws = null;

wss.on('connection', function connection(_ws) {
    ws = _ws;
});
