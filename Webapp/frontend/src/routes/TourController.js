import { useShepherd } from 'vue-shepherd';
import * as tourRefs from '@/routes/tourRefs.js';

const {
  mapTour1,
  mapTour2,
  mapTour3,
  mapTour4,
  mapTour5,
  mapTour6,
  mapTour7,
  mapTour8,
  mapTour9,
  mapTour10,
  mapHistoryTour1,
  mapHistoryTour2,
  mapHistoryTour3,
  mapHistoryTour4,
  mapHistoryTour5,
  mapHistoryTour6,
  mapHistoryTour7,
  mapHistoryTour8,
  mapHistoryTour9,
  mapHistoryTour10,
  trackerCardTour1,
  trackerCardTour2,
  trackerCardTour3,
  trackerCardTour4,
  trackerCardTour5,
  trackerCardTour6,
  trackerCardTour7,
  trackerCardTour8,
  trackerCardTour9,
  trackerCardTour10,
  trackerListTour1,
  trackerListTour2,
  trackerListTour3,
  trackerListTour4,
  trackerListTour5,
  trackerListTour6,
  trackerListTour7,
  trackerListTour8,
  trackerListTour9,
  trackerListTour10,
} = tourRefs;

export function useTour(router, t) {
  const createTour = (steps) => {
    const tour = useShepherd({
      defaultStepOptions: {
        cancelIcon: { enabled: true },
        scrollTo: { behavior: 'smooth', block: 'center' },
        highlightClass: 'shepherd-highlight',
      },
      useModalOverlay: true,
    });

    steps.forEach((step, index) => {
      if (step.ref?.value) {
        tour.addStep({
          id: step.id,
          attachTo: { element: step.ref.value, on: 'bottom' },
          text: step.text,
          highlightClass: 'shepherd-highlight',
          buttons: [
            ...(index > 0 ? [{ text: 'Previous', action: tour.back }] : []),
            ...(index < steps.length - 1 ? [{ text: 'Next', action: tour.next }] : []),
            { text: 'Close', action: tour.cancel },
          ],
        });
      } else {
        console.warn(`Element for ${step.id} is not available.`);
      }
    });

    return tour;
  };

  const createCurrentMapTour = () => createTour([
    { id: 'map-tour-1', ref: mapTour1, text: `<p>${t('Tour.CurrentMap.step1')}</p>` },
    { id: 'map-tour-2', ref: mapTour2, text: `<p>${t('Tour.CurrentMap.step2')}</p>` },
    { id: 'map-tour-3', ref: mapTour3, text: `<p>${t('Tour.CurrentMap.step3')}</p>` },
    { id: 'map-tour-4', ref: mapTour4, text: `<p>${t('Tour.CurrentMap.step4')}</p>` },
    { id: 'map-tour-5', ref: mapTour5, text: `<p>${t('Tour.CurrentMap.step5')}</p>` },
    { id: 'map-tour-6', ref: mapTour6, text: `<p>${t('Tour.CurrentMap.step6')}</p>` },
    { id: 'map-tour-7', ref: mapTour7, text: `<p>${t('Tour.CurrentMap.step7')}</p>` },
    { id: 'map-tour-8', ref: mapTour8, text: `<p>${t('Tour.CurrentMap.step8')}</p>` },
    { id: 'map-tour-9', ref: mapTour9, text: `<p>${t('Tour.CurrentMap.step9')}</p>` },
    { id: 'map-tour-10', ref: mapTour10, text: `<p>${t('Tour.CurrentMap.step10')}</p>` },
  ]);
  
  const createMapHistoryTour = () => createTour([
    { id: 'history-tour-1', ref: mapHistoryTour1, text: `<p>${t('Tour.MapHistory.step1')}</p>` },
    { id: 'history-tour-2', ref: mapHistoryTour2, text: `<p>${t('Tour.MapHistory.step2')}</p>` },
    { id: 'history-tour-3', ref: mapHistoryTour3, text: `<p>${t('Tour.MapHistory.step3')}</p>` },
    { id: 'history-tour-4', ref: mapHistoryTour4, text: `<p>${t('Tour.MapHistory.step4')}</p>` },
    { id: 'history-tour-5', ref: mapHistoryTour5, text: `<p>${t('Tour.MapHistory.step5')}</p>` },
    { id: 'history-tour-6', ref: mapHistoryTour6, text: `<p>${t('Tour.MapHistory.step6')}</p>` },
    { id: 'history-tour-7', ref: mapHistoryTour7, text: `<p>${t('Tour.MapHistory.step7')}</p>` },
    { id: 'history-tour-8', ref: mapHistoryTour8, text: `<p>${t('Tour.MapHistory.step8')}</p>` },
    { id: 'history-tour-9', ref: mapHistoryTour9, text: `<p>${t('Tour.MapHistory.step9')}</p>` },
    { id: 'history-tour-10', ref: mapHistoryTour10, text: `<p>${t('Tour.MapHistory.step10')}</p>` },
  ]);
  
  const createTrackerCardTour = () => createTour([
    { id: 'tracker-card-tour-1', ref: trackerCardTour1, text: `<p>${t('Tour.TrackerCard.step1')}</p>` },
    { id: 'tracker-card-tour-2', ref: trackerCardTour2, text: `<p>${t('Tour.TrackerCard.step2')}</p>` },
    // Add more steps similarly...
  ]);

  const createTrackerListTour = () => createTour([
    { id: 'tracker-list-tour-1', ref: trackerListTour1, text: '<p>Step 1 for Tracker List</p>' },
    // Add more steps for trackerListTour2 to trackerListTour10...
  ]);

  const startTour = (currentView) => {
    if (currentView === 'current') {
      const tour = createCurrentMapTour();
      tour.start();
    } else if (currentView === 'history') {
      const tour = createMapHistoryTour();
      tour.start();
    } else if (currentView === 'card') {
      const tour = createTrackerCardTour();
      tour.start();
    } else if (currentView === 'list') {
      const tour = createTrackerListTour();
      tour.start();
    } else {
      console.warn(`No tour defined for view: ${currentView}`);
    }
  };

  return { startTour };
}
