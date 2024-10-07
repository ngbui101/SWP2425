import { defineStore } from 'pinia';

export const useTrackerStore = defineStore('trackerStore', {
  state: () => ({
    /*State: Der Zustand des Stores, der globale Daten enthält, die von verschiedenen Komponenten verwendet werden können.*/ 
    trackers: [
      { id: 'T123', name: 'TrackerBochum', modus: 'Langzeittracking', lat: 51.48, lng: 7.21, battery: 90, accuracy: 10, temperature: 20, humidity: 45 },
      { id: 'T456', name: 'TrackerDortmund', modus: 'Echtzeittracking', lat: 51.51, lng: 7.465, battery: 85, accuracy: 12, temperature: 23, humidity: 45 },
      { id: 'T789', name: 'TrackerEssen', modus: 'Echtzeittracking', lat: 51.45, lng: 7.011, battery: 88, accuracy: 15, temperature: 22, humidity: 50 },
    ],
  }),
  
  //Methoden, um den Zustand zu ändern oder asynchrone Vorgänge wie API-Aufrufe durchzuführen
  actions: {
    addTracker(newTracker) {
      const defaultTracker = {
        lat: 50.1109,
        lng: 8.6821,
        battery: 100,
        accuracy: 5,
        temperature: 20,
        humidity: 40,
        ...newTracker,
      };
      this.trackers.push(defaultTracker);
    },

    deleteTracker(id) {
      this.trackers = this.trackers.filter(tracker => tracker.id !== id);
    },

    updateTracker(updatedTracker) {
      const index = this.trackers.findIndex(tracker => tracker.id === updatedTracker.id);
      if (index !== -1) {
        this.trackers[index] = updatedTracker; // Update tracker
      }
    },
  },

  //Ähnlich wie computed Eigenschaften in Vue, erlauben Getters es, abgeleitete Daten basierend auf dem Zustand zu erstellen.
  getters: {
    getTrackerById: (state) => (id) => state.trackers.find(tracker => tracker.id === id),
  },
});