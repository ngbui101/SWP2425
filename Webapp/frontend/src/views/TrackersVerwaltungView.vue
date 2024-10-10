<template>
    <div class="tracker-management">
      <p>Tracker Übersicht & Verwaltung </p>
  
      <!-- Form to add a new tracker -->
      <div class="form-container">
        <form @submit.prevent="addTracker">
          <div class="form-row">
            <label for="id">ID :</label>
            <input type="text" id="id" v-model="newTracker.id" />
          </div>
          <div class="form-row">
            <label for="name">Name :</label>
            <input type="text" id="name" v-model="newTracker.name" />
          </div>
          <div class="form-row">
            <label for="modus">Modus :</label>
            <select id="modus" v-model="newTracker.modus">
              <option value="Langzeittracking">Langzeittracking</option>
              <option value="Echtzeittracking">Echtzeittracking</option>
            </select>
          </div>
          <button type="submit">Neuen Tracker hinzufügen</button>
        </form>
      </div>
  
      <!-- Suchfeld für das Filtern nach einer bestimmten ID -->
      <div class="search-container">
        <input type="text" v-model="searchId" placeholder="Suche nach ID" />
      </div>
  
      <!-- Liste aller Tracker -->
      <div class="trackers-list">
        <ul>
          <li v-for="tracker in filteredTrackers" :key="tracker.id" class="tracker-item">
            <div class="tracker-info">
              <strong>ID :</strong> {{ tracker.id }} 
              <strong>|  Name :</strong> {{ tracker.name }} 
              <strong>|  Modus :</strong> 
              <select v-model="tracker.modus" @change="updateModus(tracker)">
                <option value="Langzeittracking">Langzeittracking</option>
                <option value="Echtzeittracking">Echtzeittracking</option>
              </select>
            </div>
            <div class="tracker-actions">
              <button @click="deleteTracker(tracker.id)" class="delete-btn">Löschen</button>
            </div>
          </li>
        </ul>
      </div>
    </div>
  </template>
  
  
  <script>
  import { useTrackerStore } from '@/stores/trackerStore'; // Import your tracker store
  import { computed, ref } from 'vue'; // Import computed and ref for reactivity
  
  export default {
    setup() {
      const trackerStore = useTrackerStore(); // Access the store
  
      // Reactive data for the new tracker form
      const newTracker = ref({
        id: '',
        name: '',
        modus: '',
      });
  
      const searchId = ref('');
  
      // Filtered list of trackers based on the search ID
      const filteredTrackers = computed(() => {
        return trackerStore.trackers.filter(tracker =>
          tracker.id.includes(searchId.value)
        );
      });
  
      // Add a new tracker
      const addTracker = () => {
        // Check if the ID already exists
        if (trackerStore.getTrackerById(newTracker.value.id)) {
          alert('Ein Tracker mit dieser ID existiert bereits!');
          return;
        }
  
        // Add tracker using the store's action
        trackerStore.addTracker({
          ...newTracker.value,
          latitude: 52.52, // Default value
          longitude: 13.405, // Default value
          location: 'Berlin', // Default value
          battery: 90, // Default value
          accuracy: 10 // Default value
        });
  
        // Reset form
        newTracker.value.id = '';
        newTracker.value.name = '';
        newTracker.value.modus = '';
      };
  
      // Update the modus of an existing tracker
      const updateModus = (tracker) => {
        trackerStore.updateTracker(tracker);
      };
  
      // Delete a tracker
      const deleteTracker = (id) => {
        if (confirm('Sind Sie sicher, dass Sie den Tracker löschen möchten?')) {
          trackerStore.deleteTracker(id);
        }
      };
  
      return {
        newTracker,
        searchId,
        filteredTrackers,
        addTracker,
        updateModus,
        deleteTracker,
      };
    },
  };
  </script>
  
  <style scoped>
 .tracker-management {
  text-align: center;
  margin-top: 20px;
}
 

p {
  font-size: 26px;
  font-weight: bold;
  margin-bottom: 20px;
  color: #00543D;
}

.form-container {
  background-color: #fafafa;
  border: 1px solid #e0e0e0;
  padding: 20px;
  margin-bottom: 20px;
  border-radius: 10px;
  box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
  display: inline-block;
  
}

.form-row {
  margin: 10px 0;
  display: flex;
  justify-content: space-between;
  align-items: center;
  
}

label {
  font-weight: bold;
  margin-right: 10px;
  flex-basis: 30%;
  text-align: right;
}

input, select {
  padding: 10px;
  border-radius: 5px;
  border: 1px solid #ddd;
  flex-basis: 65%;
}

button {
  background-color: #00543D;
  color: white;
  border-radius: 5px;
  padding: 10px 20px;
  margin-top: 10px;
  border: none;
  cursor: pointer;
  font-size: 14px;
}

button:hover {
  background-color: #007F5F;
}

.search-container {
  margin: 20px 0;
  width: 90%; /* Gleiche Breite */
  margin: 0 auto; /* Zentriert den Container */

}

input[type="text"] {
  padding: 10px;
  width: 50%;
  margin: 0 auto;
  display: block;
  border: 1px solid #ddd;
  border-radius: 5px;
}

.trackers-list {
  background-color: rgba(255, 255, 255, 0);
  padding: 10px;
  border-radius: 10px;
  border: 1px solid #fafafa; /* 2px wide, solid style, and red color */
  box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
  width: 47%; /* Gleiche Breite */
  margin: 0 auto; /* Zentriert den Container */

}

ul {
  list-style: none;
  padding: 0;
  margin: 0;
}
.tracker-info {
  display: center;
  justify-content: space-between;
  align-items: center; /* Zentriert die Labels und Werte vertikal */
  gap: 30px; /* Abstand zwischen den Elementen, falls gewünscht */
}

.tracker-info strong {
  display: inline-block; /* Sicherstellen, dass das Label und der Wert in einer Linie sind */
  margin-right: 5px; /* Geringer Abstand zwischen Label und Wert */
  white-space: nowrap; /* Verhindert Zeilenumbruch bei langen Texten */
}

.tracker-info select {
  padding: 5px;
  font-size: 14px;
}

.tracker-item {
  display: flex;
  justify-content: space-between;
  align-items: center; /* Zentriert alle Elemente in der Höhe */
  background-color: rgba(255, 255, 255, 0.3); /* Transparente Hintergrundfarbe */
  margin: 10px 0;
  padding: 10px;
  border-radius: 5px;
  box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1); /* Schatten für bessere Sichtbarkeit */
}

.tracker-actions {
  display: flex;
  align-items: center; /* Zentriert den Löschen-Button vertikal */
  justify-content: flex-end;
}

.delete-btn {
  background-color: #00543D;
  color: white;
  border: none;
  border-radius: 5px;
  padding: 10px;
  cursor: pointer;
  margin: 5px;
  align-items: center; /* Zentriert alle Elemente in der Höhe */

}

.delete-btn:hover {
  background-color: #007F5F;
}
  </style>
  