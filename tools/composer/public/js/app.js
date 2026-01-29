// State
let components = [];
let cy = null;
let currentDir = '';

// DOM Elements
const dirPathInput = document.getElementById('dir-path');
const btnOpen = document.getElementById('btn-open');
const btnCreate = document.getElementById('btn-create');
const btnSave = document.getElementById('btn-save');
const editorContent = document.getElementById('editor-content');
const selectedComponentName = document.getElementById('selected-component-name');

// ... (initCy, File Browser Logic, etc. remain unchanged)

// Create New File
btnCreate.addEventListener('click', () => {
    if (!currentDir) return alert('Please open a directory first.');

    const filename = prompt('Enter filename for new component (e.g. "my_component"):');
    if (!filename) return;

    fetch('/api/create', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ dirPath: currentDir, filename: filename })
    })
        .then(res => res.json())
        .then(data => {
            if (data.error) {
                alert('Error creating file: ' + data.error);
            } else {
                alert('File created successfully!');
                // Reload files to show the new component
                loadFiles(currentDir);
            }
        })
        .catch(err => alert('Request failed: ' + err.message));
});


// 3. Render Graph
function renderGraph() {
    cy.elements().remove();
    const elements = [];

    // Create Nodes
    components.forEach((comp, index) => {
        elements.push({
            group: 'nodes',
            data: {
                id: comp.filename,
                // Clean label: remove extension
                label: comp.filename.replace(/\.json$/i, ''),
                originalIndex: index
            }
        });
    });

    // Create Edges based on Dataports
    // Logic: Connect if same dataport key AND compatible socket types (pub-sub, push-pull)
    const socketPairs = {
        'pub': 'sub',
        'push': 'pull',
        'req': 'rep'
    };

    components.forEach(sourceComp => {
        if (!sourceComp.data.dataport) return;

        Object.keys(sourceComp.data.dataport).forEach(portName => {
            const sourcePort = sourceComp.data.dataport[portName];
            const sourceType = sourcePort.socket_type;

            // Check if this is a "Sender" type
            if (socketPairs.hasOwnProperty(sourceType)) {
                const targetType = socketPairs[sourceType];

                // Search for "Receiver" types in other components
                components.forEach(targetComp => {
                    if (sourceComp === targetComp) return;
                    if (!targetComp.data.dataport) return;

                    const targetPort = targetComp.data.dataport[portName]; // Match by KEY NAME

                    if (targetPort && targetPort.socket_type === targetType) {
                        elements.push({
                            group: 'edges',
                            data: {
                                source: sourceComp.filename,
                                target: targetComp.filename,
                                label: portName,
                                type: sourceType
                            }
                        });
                    }
                });
            }
        });
    });

    cy.add(elements);
    cy.layout({
        name: 'cose',
        animate: true,
        componentSpacing: 100,
        nodeOverlap: 20
    }).run();
}

// 4. Property Editor
function loadProperties(filename) {
    const comp = components.find(c => c.filename === filename);
    if (!comp) return;

    selectedComponentName.textContent = filename;
    editorContent.innerHTML = ''; // Clear

    // Helper to refresh panel
    const refresh = () => loadProperties(filename);

    // Create Root Fields (General)
    const rootSection = createSection('General', comp.data, null, refresh);
    // Hardcoded known general fields, but user might want to add others
    // We iterate current keys that are NOT 'parameters' or 'dataport'
    Object.keys(comp.data).forEach(key => {
        if (key !== 'parameters' && key !== 'dataport' && typeof comp.data[key] !== 'object') {
            createInput(rootSection, comp.data, key, null, refresh);
        }
    });
    editorContent.appendChild(rootSection);

    // Create Parameters Section
    if (!comp.data.parameters) comp.data.parameters = {};
    const paramSection = createSection('Parameters', comp.data.parameters, null, refresh);
    Object.keys(comp.data.parameters).forEach(key => {
        createInput(paramSection, comp.data.parameters, key, null, refresh);
    });
    editorContent.appendChild(paramSection);

    // Create Dataport Section
    if (!comp.data.dataport) comp.data.dataport = {};
    const portSection = createSection('Dataports', comp.data.dataport, 'object', refresh);

    Object.keys(comp.data.dataport).forEach(key => {
        const portData = comp.data.dataport[key];
        const portContainer = document.createElement('div');
        portContainer.className = 'dataport-item';

        const header = document.createElement('div');
        header.className = 'dataport-header';

        const titleSpan = document.createElement('span');
        titleSpan.textContent = key;
        header.appendChild(titleSpan);

        // Remove Port Button
        const btnRemovePort = document.createElement('button');
        btnRemovePort.className = 'btn-icon remove';
        btnRemovePort.innerHTML = '&times;';
        btnRemovePort.title = 'Remove Dataport';
        btnRemovePort.onclick = () => {
            if (confirm(`Remove dataport "${key}"?`)) {
                delete comp.data.dataport[key];
                refresh();
            }
        };
        header.appendChild(btnRemovePort);
        portContainer.appendChild(header);

        // Add Property to Port Button
        const btnAddProp = document.createElement('button');
        btnAddProp.className = 'btn-sm add-prop';
        btnAddProp.textContent = '+ Add Property';
        btnAddProp.onclick = () => {
            const propName = prompt('New Property Name (e.g. host):');
            if (propName && !portData.hasOwnProperty(propName)) {
                portData[propName] = '';
                refresh();
            }
        };
        portContainer.appendChild(btnAddProp);


        // Edit fields of the port
        Object.keys(portData).forEach(field => {
            if (typeof portData[field] !== 'object') {
                createInput(portContainer, portData, field, null, refresh);
            } else if (field === 'resolution' && typeof portData[field] === 'object') {
                const subSection = document.createElement('div');
                subSection.style.paddingLeft = '10px';
                subSection.style.borderLeft = '2px solid #ddd';
                subSection.style.marginTop = '5px';
                const subHeader = document.createElement('div');
                subHeader.innerHTML = `<strong>${field}</strong>`;
                subSection.appendChild(subHeader);
                Object.keys(portData[field]).forEach(subKey => {
                    createInput(subSection, portData[field], subKey, null, refresh);
                });
                portContainer.appendChild(subSection);
            }
        });

        portSection.appendChild(portContainer);
    });
    editorContent.appendChild(portSection);
}

function clearProperties() {
    selectedComponentName.textContent = 'No component selected';
    editorContent.innerHTML = '<p style="padding:10px; color:#999;">Select a component to edit its properties.</p>';
}

function createSection(title, targetObj, addType, refreshCallback) {
    const div = document.createElement('div');
    div.className = 'section';

    const header = document.createElement('div');
    header.className = 'section-header';
    header.style.display = 'flex';
    header.style.justifyContent = 'space-between';
    header.style.alignItems = 'center';
    header.style.marginBottom = '10px';
    header.style.borderBottom = '2px solid #e67e22';

    const h4 = document.createElement('h4');
    h4.style.margin = '0';
    h4.style.border = 'none';
    h4.textContent = title;
    header.appendChild(h4);

    // Add Button
    const btnAdd = document.createElement('button');
    btnAdd.className = 'btn-icon add';
    btnAdd.textContent = '+';
    btnAdd.style.cursor = 'pointer';
    btnAdd.style.fontWeight = 'bold';
    btnAdd.title = `Add Item to ${title}`;
    btnAdd.onclick = () => {
        const key = prompt(`Enter name for new ${title} item:`);
        if (key) {
            if (title === 'Dataports') {
                // For dataports, add a default object
                if (!targetObj[key]) targetObj[key] = { transport: 'tcp', socket_type: 'pub', port: 0 };
            } else {
                // For General/Parameters, add string/number
                if (!targetObj.hasOwnProperty(key)) targetObj[key] = '';
            }
            refreshCallback(); // Re-render
        }
    };
    header.appendChild(btnAdd);

    div.appendChild(header);
    return div;
}

function createInput(container, refObj, key, forcedType, refreshCallback) {
    const formGroup = document.createElement('div');
    formGroup.className = 'form-group';
    formGroup.style.position = 'relative';
    formGroup.style.display = 'flex';
    formGroup.style.alignItems = 'center';
    formGroup.style.gap = '5px';

    const label = document.createElement('label');
    label.textContent = key;
    label.style.width = '80px';
    label.style.flexShrink = '0';
    formGroup.appendChild(label);

    const val = refObj[key];
    const type = forcedType || (typeof val === 'number' ? 'number' : 'text');

    const input = document.createElement('input');
    input.type = type;
    input.value = val;
    input.style.flexGrow = '1';

    input.addEventListener('change', (e) => {
        let value = e.target.value;
        if (type === 'number') value = Number(value);
        refObj[key] = value;
        // Optionally update graph if pertinent fields changed
        if (key === 'socket_type' || key === 'transport' || key === 'port') {
            renderGraph();
        }
    });

    formGroup.appendChild(input);

    // Remove Button
    const btnRemove = document.createElement('button');
    btnRemove.className = 'btn-icon remove-small';
    btnRemove.innerHTML = '&minus;';
    btnRemove.title = 'Remove Property';
    btnRemove.style.flexShrink = '0';
    btnRemove.style.cursor = 'pointer';
    btnRemove.style.backgroundColor = '#ffdddd';
    btnRemove.style.border = '1px solid #ffaaaa';
    btnRemove.onclick = () => {
        delete refObj[key];
        refreshCallback();
    };
    // Ensure we don't accidentally submit if inside form, though there is no form
    btnRemove.type = 'button';
    formGroup.appendChild(btnRemove);

    container.appendChild(formGroup);
}

// 5. Save Data
btnSave.addEventListener('click', () => {
    if (components.length === 0) return;

    console.log('Preparing to save...');
    const updates = components.map(c => ({
        path: c.path,
        data: c.data
    }));

    // Debug log for first component's dataport triggers
    if (updates.length > 0 && updates[0].data.dataport) {
        console.log('First component dataport state:', updates[0].data.dataport);
    }

    fetch('/api/save', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ updates })
    })
        .then(res => res.json())
        .then(data => {
            if (data.errors) {
                alert('Saved with some errors. Check console for details.');
                console.error('Save errors:', data.errors);
            } else {
                console.log('Server response:', data);
                alert(`Saved successfully! Updated ${data.results ? data.results.length : 0} files.`);
            }
        })
        .catch(err => {
            console.error('Save failed:', err);
            alert('Error saving: ' + err.message);
        });
});

// Start
initCy();
clearProperties();
