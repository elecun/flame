const express = require('express');
const bodyParser = require('body-parser');
const cors = require('cors');
const fs = require('fs');
const path = require('path');

const app = express();
const PORT = 3000;

app.use(cors());
app.use(bodyParser.json({ limit: '50mb' })); // Increase limit for large JSONs
app.use(express.static('public'));

// API: List directories for file browser
app.get('/api/browse', (req, res) => {
    // Default to current working directory or provided path
    let reqPath = req.query.path || process.cwd();

    // Safety check for empty path if something goes wrong, default to /
    if (!reqPath) reqPath = '/';

    try {
        if (!fs.existsSync(reqPath)) {
            return res.status(404).json({ error: 'Path not found' });
        }

        const items = fs.readdirSync(reqPath, { withFileTypes: true });

        // Filter only directories, mapping to simple objects
        const directories = items
            .filter(item => item.isDirectory())
            .map(item => item.name)
            .filter(name => !name.startsWith('.')) // Optional: Hide dotfiles
            .sort();

        // Get parent directory
        const parent = path.resolve(reqPath, '..');

        res.json({
            current: path.resolve(reqPath),
            parent: parent === reqPath ? null : parent, // If root, no parent
            directories: directories
        });
    } catch (err) {
        res.status(500).json({ error: 'Access denied or invalid path', details: err.message });
    }
});

// API: List JSON files in a directory
app.get('/api/load', (req, res) => {
    const dirPath = req.query.path;

    if (!dirPath) {
        return res.status(400).json({ error: 'Missing path parameter' });
    }

    if (!fs.existsSync(dirPath)) {
        return res.status(404).json({ error: 'Directory not found' });
    }

    try {
        const files = fs.readdirSync(dirPath);
        const jsonFiles = files.filter(file => file.endsWith('.json'));
        const components = [];

        jsonFiles.forEach(file => {
            const filePath = path.join(dirPath, file);
            try {
                const content = fs.readFileSync(filePath, 'utf8');
                const jsonData = JSON.parse(content);
                components.push({
                    filename: file,
                    path: filePath,
                    data: jsonData
                });
            } catch (err) {
                console.error(`Error reading file ${file}:`, err);
            }
        });

        res.json({ components });
    } catch (err) {
        res.status(500).json({ error: err.message });
    }
});

// API: Save updated JSON files
app.post('/api/save', (req, res) => {
    const updates = req.body.updates; // Expects an array of { path, data }

    console.log('Received save request.');
    if (updates) {
        console.log(`Number of files to update: ${updates.length}`);
        if (updates.length > 0) {
            console.log('Sample update data (first file):', JSON.stringify(updates[0].data.dataport, null, 2));
        }
    } else {
        console.error('No updates found in body:', req.body);
    }

    if (!updates || !Array.isArray(updates)) {
        return res.status(400).json({ error: 'Invalid updates format' });
    }


    const results = [];
    const errors = [];

    updates.forEach(update => {
        try {
            // Write structured JSON back to file
            fs.writeFileSync(update.path, JSON.stringify(update.data, null, 4), 'utf8');
            results.push(update.path);
        } catch (err) {
            console.error(`Error writing file ${update.path}:`, err);
            errors.push({ path: update.path, error: err.message });
        }
    });

    if (errors.length > 0) {
        res.status(207).json({ message: 'Partial success', results, errors });
    } else {
        res.json({ message: 'All files saved successfully', results });
    }
});

// API: Create new JSON file
app.post('/api/create', (req, res) => {
    const { dirPath, filename } = req.body;

    if (!dirPath || !filename) {
        return res.status(400).json({ error: 'Missing directory path or filename' });
    }

    // Ensure extension
    let safeFilename = filename.trim();
    if (!safeFilename.toLowerCase().endsWith('.json')) {
        safeFilename += '.json';
    }

    const filePath = path.join(dirPath, safeFilename);

    if (fs.existsSync(filePath)) {
        return res.status(409).json({ error: 'File already exists' });
    }

    try {
        // Create empty component template
        const template = {
            rt_cycle_ns: 1000000000,
            verbose: 1,
            parameters: {},
            dataport: {}
        };

        fs.writeFileSync(filePath, JSON.stringify(template, null, 4), 'utf8');
        res.json({ message: 'File created successfully', path: filePath });
    } catch (err) {
        res.status(500).json({ error: 'Failed to create file', details: err.message });
    }
});

app.listen(PORT, () => {
    console.log(`Composer Server running at http://localhost:${PORT}`);
});
