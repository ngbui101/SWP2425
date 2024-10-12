import { useShepherd } from 'vue-shepherd';
import { tour1, tour2 } from '@/routes/tourRefs.js';  // Import the shared refs


export function useTour(router) {
  const totalSteps = 2;  // Define the total number of steps

  const tour = useShepherd({
    defaultStepOptions: {
      cancelIcon: { enabled: true },
      scrollTo: { behavior: 'smooth', block: 'center' },
      highlightClass: 'shepherd-highlight',  // Custom highlight class
    },
    useModalOverlay: true,
  });

  // Add the first step
  if (tour1.value) {
    tour.addStep({
      id: 'logo-step',
      attachTo: {
        element: tour1.value, 
        on: 'bottom',
      },
      text: `
        <div>
          <p>Welcome to BO-Tracker</p>
          <p>In this guided tour, we will guide you through the application and highlight all the important things.</p>
          <p>You can always close this tour by clicking the close button and pick it up another time, watch previous steps, or skip to the next step.</p>
          <p>If you have any questions, feel free to contact us.</p>
          <p>Your BO-Tracker team</p>
        </div>
        <footer style="text-align: center; margin-top: 10px;">1/${totalSteps}</footer>  <!-- Pagination footer -->
      `,
      buttons: [
        {
          text: 'Next',  // Next button for moving to the second step
          action: tour.next,
        },
        {
          text: 'Close',
          action: tour.cancel,
        },
      ],
      highlightClass: 'shepherd-highlight',  // Custom highlight class
    });
  } else {
    console.warn('tour1 element is not available for the tour.');
  }

  // Add the second step
  if (tour2.value) {
    tour.addStep({
      id: 'second-step',
      attachTo: {
        element: tour2.value, 
        on: 'bottom',
      },
      text: `
        <div>
          <p>Let's start by adding your tracker to this application.</p>
        </div>
        <footer style="text-align: center; margin-top: 10px;">2/${totalSteps}</footer>  <!-- Pagination footer -->
      `,
      buttons: [
        {
          text: 'Previous',  // Previous button for going back to the first step
          action: tour.back,
        },
        {
          text: 'Next',  // Next button for further steps (if any)
          action: () => {
            router.push({ name: 'trackers' });  // Navigate to /trackers
            tour.next();  // Continue to the next step in the tour
          },
        },
        {
          text: 'Close',
          action: tour.cancel,
        },
      ],
      highlightClass: 'shepherd-highlight',  // Custom highlight class
    });
  } else {
    console.warn('tour2 element is not available for the tour.');
  }

  // Start the tour
  tour.start();
}
