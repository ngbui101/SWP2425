<template>
    <div :class="['map-view', (user.settings?.template ?? 'default') === 'dark' ? 'dark-mode' : '']">
        <!-- Toolbar with Toggle Bar for Current/History view -->
        <div class="toolbar">
            <div class="toggle-container" @mousedown="startDragging($event)" @mouseup="stopDragging"
                @mouseleave="stopDragging" @mousemove="handleDragging($event)">

                <!-- Sliding background div -->
                <div class="slider"
                    :class="{ 'slide-left': currentView === 'current', 'slide-right': currentView === 'history' }">
                </div>

                <!-- Current Map View Toggle (Left) -->
                <label @click="setView('current')" :class="{ active: currentView === 'current' }">
                    {{ $t('MapView-Current') }}
                </label>


                <!-- Map History View Toggle (Right) -->
                <label @click="setView('history')" :class="{ active: currentView === 'history' }">
                    {{ $t('MapView-History') }}
                </label>
            </div>
        </div>

        <!-- Conditional rendering based on selected view -->
        <CurrentMap v-if="currentView === 'current'" />
        <MapHistory v-if="currentView === 'history'" />
    </div>
</template>

<script setup>

import { ref, computed } from 'vue';
import CurrentMap from './components/CurrentMap.vue';
import MapHistory from './components/MapHistory.vue';
import { useAuthStore, currentView } from "@/stores/auth";
import { onMounted } from 'vue';
onMounted(async () => {


    currentView.value = 'current';
});

let isDragging = ref(false); // Indicates if the slider is being dragged
let sliderPosition = ref(100);
let draggingDirection = ref(''); // Tracks the drag direction

// Fetch user from the auth store
const authStore = useAuthStore();
const user = computed(() => authStore.userDetail);

// Method to change the view
const setView = (view) => {
    currentView.value = view;
    sliderPosition.value = view === 'current' ? 0 : 100; // Current Map on left (0), Map History on right (100)
};

// Start dragging the slider
const startDragging = (event) => {
    isDragging.value = true;
    updateSliderPosition(event.clientX);
};

// Stop dragging the slider
const stopDragging = () => {
    if (!isDragging.value) return;
    isDragging.value = false;

    // Determine which view is closer to the slider
    if (sliderPosition.value > 50) {
        setView('history'); // Snap to Map History (right side)
    } else {
        setView('current'); // Snap to Current Map (left side)
    }

    // Reset dragging direction after stop
    draggingDirection.value = '';
};

// Handle the dragging movement
const handleDragging = (event) => {
    if (!isDragging.value) return;

    const oldSliderPosition = sliderPosition.value;
    updateSliderPosition(event.clientX);

    // Set the dragging direction based on the new position
    draggingDirection.value = sliderPosition.value > oldSliderPosition ? 'right' : 'left';
};

// Update the slider's position based on the mouse X position
const updateSliderPosition = (clientX) => {
    const container = document.querySelector('.toggle-container');
    const rect = container.getBoundingClientRect();
    const percentage = ((clientX - rect.left) / rect.width) * 100;

    // Boundaries to prevent dragging outside
    sliderPosition.value = Math.min(100, Math.max(0, percentage));
};
</script>

<style scoped>
.map-view {
    background: linear-gradient(135deg, #f1e4cc 0%, #e6cc99 50%, #f1e4cc 100%);
    padding-bottom: 20px;
    min-height: 100vh;
    padding-top: 15px;
    margin: 0 auto;
}

@media (max-width: 768px) {
    .map-view {
        padding-right: 20px;
        /* Adds padding to create margin on the sides */

        /* Adds margin on both sides */
        box-sizing: border-box;
        /* Ensures padding doesn’t expand width */
    }
}

.map-view.dark-mode {
    background: linear-gradient(135deg, #1e1e1e 0%, #141414 50%, #1e1e1e 100%);
}

/* Toolbar styling */
.toolbar {
    display: flex;
    justify-content: center;
    align-items: center;
    margin-top: 10px;
    margin-bottom: 20px;
}

/* Toggle container */
.toggle-container {
    position: relative;
    width: 250px;
    height: 50px;
    border-radius: 30px;
    border: 1px solid #000;
    display: flex;
    align-items: center;
    justify-content: space-between;
    background-color: white;
    overflow: hidden;
    user-select: none;
}

/* Sliding background */
.slider {
    position: absolute;
    top: 0;
    bottom: 0;
    width: 50%;
    background-color: #C19A6B;
    border-radius: 30px 0 0 30px;
    transition: transform 0s ease;
    z-index: 0;
    transform: translateX(calc(var(--slider-pos, 0) * 1%));
}

.slide-left {
    transform: translateX(0);
}

.slide-right {
    border-radius: 0 30px 30px 0;
    transform: translateX(100%);
}

/* Toggle labels */
.toggle-container label {
    width: 50%;
    text-align: center;
    font-weight: bold;
    padding: 10px 0;
    cursor: pointer;
    z-index: 1;
    /* Ensures text is on top of slider */
}

.toggle-container .active {
    color: rgb(0, 0, 0);
    /* Color of active label */
    border-right: none;
    /* Remove right border when active */
}

/* Remove left border on Current Map View when active */
.toggle-container label:last-of-type.active {
    border-left: none;
}

.toggle-container label:not(.active) {
    color: #1f1f1f;
    border-left: none;
}

/* Dark mode styles */
.dark-mode .toggle-container {
    background-color: #333;
    border-color: #555;
}

.dark-mode .slider {
    background-color: #5A976D;
}

.dark-mode .toggle-container label {
    color: white;
}

/* Add other styles back */
.dark-mode .toggle-container .active {
    background-color: #5A976D;
    color: #333;
}

.dark-mode .toggle-container label:not(.active) {
    color: #aaa;
}
</style>
