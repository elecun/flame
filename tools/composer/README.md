# flame_osm Composer

A web-based tool for visually editing and managing `flame_osm` component profile (`.json`) files.

## Features

*   **Load Profiles**: Load all component JSON files from a specified directory.
*   **Visual Graph**: Visualize components and their `dataport` connections using a node-link diagram.
*   **Property Editor**: Edit general parameters, custom parameters, and dataport configurations via a GUI.
*   **Save & Update**: Save changes back to the original JSON files with a single click.

## Prerequisites

*   Node.js (v12 or higher)
*   npm

## Installation

1.  Navigate to the tool directory:
    ```bash
    cd flame/tools/composer
    ```
2.  Install dependencies:
    ```bash
    npm install
    ```

## Usage

1.  Start the server:
    ```bash
    node server.js
    ```
    *   The server will start at `http://localhost:3000`.

2.  Open your web browser and navigate to `http://localhost:3000`.

3.  **Loading Files**:
    *   In the top bar input field, enter the **absolute path** to the directory containing your JSON component files (e.g., `/home/user/dev/flame_osm/bin/x86_64/osm`).
    *   Click **Open**.

4.  **Editing**:
    *   **Drag** nodes to rearrange the graph.
    *   **Click** on a node to view and edit its properties in the right-side panel.
    *   Modify values in the input fields.

5.  **Saving**:
    *   Click **Save & Update** in the top bar to apply changes to the files on disk.

## Project Structure

*   `server.js`: Node.js Express backend. Handles file I/O operations.
*   `public/`: Static frontend assets.
    *   `index.html`: Main UI layout.
    *   `js/app.js`: Frontend logic (Cytoscape graph, state management, API calls).
    *   `css/style.css`: Styling.

## Dependencies

*   **Backend**: `express`, `body-parser`, `cors`
*   **Frontend**: `cytoscape` (loaded via CDN)
