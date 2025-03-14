<template>
    <div class="popup-overlay" @click.self="closePopup">
        <div class="popup-card" :class="[(template ?? 'default') === 'dark' ? 'dark-mode' : '']">
            <div class="popup-header">
                <h2> {{ $t("TimestampFilters") }} </h2>
                <button class="close-btn" @click="closePopup">✖</button>
            </div>

            <div class="popup-body">
                <!-- Mode Filter -->
                <div class="popup-section">
                    <h3>{{ $t("Mode")}}</h3>
                    <label>
                        <input type="checkbox" v-model="realTimeChecked" /> GPS
                    </label>
                    <label>
                        <input type="checkbox" v-model="longTimeChecked" /> LTE
                    </label>
                    <label>
                        <input type="checkbox" v-model="nbiotChecked" /> NBIOT
                    </label>
                    <label>
                        <input type="checkbox" v-model="gsmChecked" /> GSM
                    </label>
                </div>
            </div>

            <div class="popup-footer">
                <button class="popup-save-btn" @click="applyFilters">{{ $t("ApplyFilters") }}</button>
            </div>
        </div>
    </div>
</template>

<script setup>
import { ref, watch } from 'vue';
import axios from 'axios';
import { useAuthStore } from '@/stores/auth';

const authStore = useAuthStore();

const props = defineProps({
    template: String,
    filters: Object,
    closePopup: Function,
    applyFilters: Function,
});

// Separate properties for each filter
const realTimeChecked = ref(props.filters.mode.includes('RT'));
const longTimeChecked = ref(props.filters.mode.includes('LT'));
const nbiotChecked = ref(props.filters.mode.includes('NBIOT'));
const gsmChecked = ref(props.filters.mode.includes('GSM'));

// Sync the changes to filters.mode based on the checkboxes
const filters = ref({
    mode: [...props.filters.mode],
    validPosition: props.filters.validPosition || false
});

watch([realTimeChecked, longTimeChecked, nbiotChecked, gsmChecked], () => {
    filters.value.mode = [];
    if (realTimeChecked.value) filters.value.mode.push('RT');
    if (longTimeChecked.value) filters.value.mode.push('LT');
    if (nbiotChecked.value) filters.value.mode.push('NBIOT');
    if (gsmChecked.value) filters.value.mode.push('GSM');
});

const applyFilters = async () => {
    try {
        console.log('Filters before sending:', filters.value.mode); // Debugging line

        const token = authStore.accessToken;
        const config = {
            headers: {
                Authorization: `Bearer ${token}`,
            },
        };

        // Send updated timestamp filters to backend
        await axios.put(
            'http://localhost:3500/api/settings',
            {
                timestampFilters: {
                    mode: filters.value.mode, // Selected modes (LTE, GPS, NBIOT, GSM)
                    validPosition: filters.value.validPosition, // Valid position filter
                },
            },
            config
        );

        console.log('Filters sent to backend:', filters.value.mode); // Debugging line

        // Refresh user data to get the latest settings
        await authStore.getUser();

        // Apply updated filters in the parent and close popup
        props.applyFilters(filters.value);
        props.closePopup();
    } catch (error) {
        console.error('Failed to update timestamp filters:', error);
    }
};

</script>


<style scoped>
.popup-overlay {
    position: fixed;
    top: 0;
    left: 0;
    width: 100%;
    height: 100%;
    background-color: rgba(0, 0, 0, 0.5);
    display: flex;
    justify-content: center;
    align-items: center;
    z-index: 1000;
}

.popup-card {
    background: linear-gradient(135deg, #f1e4cc 0%, #e6cc99 50%, #f1e4cc 100%);
    padding: 20px;
    border: 1px solid #ddd;
    border-radius: 12px;
    width: 320px;
    max-width: 90vw;
    box-shadow: 0 5px 10px rgba(0, 0, 0, 1);
    text-align: center;
    transition: transform 0.3s, box-shadow 0.3s;
    position: relative;
}

@media screen and (min-width: 768px) {
    .popup-card {
        width: 400px;
    }
}

.dark-mode.popup-card {
    background: linear-gradient(135deg, #1e1e1e 0%, #141414 50%, #1e1e1e 100%);
    border-color: #555;
    color: #bbb;
    box-shadow: 0 4px 8px rgba(255, 255, 255, 0.1);
}

.popup-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-bottom: 10px;
}

.popup-header h2 {
    font-size: 1.5rem;
    margin: 0;
}

.close-btn {
    background: none;
    border: none;
    font-size: 1.5rem;
    cursor: pointer;
    color: #851515;
}

/* Default mode styling */
input[type="checkbox"] {
    accent-color: #00543D;
    /* Default color */
}

/* Dark mode styling */
.dark-mode input[type="checkbox"] {
    accent-color: #E69543;
    /* Dark mode color */
}

.close-btn:hover {
    color: #750f0f;
}

.dark-mode .close-btn {
    color: #E69543;
}

.dark-mode .close-btn:hover {
    color: #cc7e36;
}

.popup-body {
    display: flex;
    flex-direction: column;
    gap: 15px;
}

.popup-section {
    text-align: left;
}

.popup-section label {
    font-size: 0.9rem;
    display: block;
    margin-bottom: 5px;
}

.popup-footer {
    margin-top: 20px;
}

.popup-save-btn {
    background-color: #851515;
    color: #fff;
    padding: 12px 30px;
    font-size: 1rem;
    font-weight: bold;
    border-radius: 6px;
    border: none;
    cursor: pointer;
    transition: background-color 0.3s ease, transform 0.2s;
}

.popup-save-btn:hover {
    background-color: #750f0f;
    transform: scale(1.05);
}

.dark-mode .popup-save-btn {
    background-color: #E69543;
    color: #1f1f1f;
}

.dark-mode .popup-save-btn:hover {
    background-color: #cc7e36;
}
</style>