import { useShepherd } from 'vue-shepherd';
import { tour1, tour2, tour3 } from '@/routes/tourRefs.js'; // Import the shared refs

export function useTour(router) {
  const totalSteps = 3; // Define the total number of steps

  const tour = useShepherd({
    defaultStepOptions: {
      cancelIcon: { enabled: true },
      scrollTo: { behavior: 'smooth', block: 'center' },
      highlightClass: 'shepherd-highlight', // Custom highlight class
    },
    useModalOverlay: true,
  });

  // Utility function to wait for an element to exist in the DOM
  function waitForElement(ref, timeout = 5000) {
    return new Promise((resolve, reject) => {
      const startTime = Date.now();
      const interval = setInterval(() => {
        if (ref.value) {
          clearInterval(interval);
          resolve();
        } else if (Date.now() - startTime > timeout) {
          clearInterval(interval);
          reject(new Error('Element not found within the timeout period'));
        }
      }, 100);
    });
  }

  // Function to restart the tour and jump to a specific step
  async function restartTourAtStep(stepIndex) {
    tour.cancel(); // Cancel any existing tour instance
    await tour.start(); // Restart the tour
    for (let i = 0; i < stepIndex; i++) {
      tour.next(); // Move to the desired step
    }
  }

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
          text: 'Next', // Next button for moving to the second step
          action: tour.next,
        },
        {
          text: 'Close',
          action: tour.cancel,
        },
      ],
      highlightClass: 'shepherd-highlight', // Custom highlight class
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
          text: 'Previous',
          action: tour.back,
        },
        {
          text: 'Next',
          action: async () => {
            try {
              await router.push({ name: 'trackers' }); // Navigate to /trackers
              await waitForElement(tour3, 5000); // Wait for the element to exist (timeout set to 5000ms)
              await restartTourAtStep(2); // Restart the tour and move to step 3
            } catch (error) {
              console.error('Error during navigation or waiting for the element:', error);
              alert('The required element for the tour is not available. Please try restarting the tour.');
              tour.cancel(); // Cancel the tour if an error occurs
            }
          },
        },
        {
          text: 'Close',
          action: tour.cancel,
        },
      ],
      highlightClass: 'shepherd-highlight',
    });
  } else {
    console.warn('tour2 element is not available for the tour.');
  }

  // Add the third step
  if (tour3.value) {
    tour.addStep({
      id: 'third-step',
      attachTo: {
        element: tour3.value,
        on: 'bottom',
      },
      text: `
        <div>
          <p>This button allows you to add your tracker to this application. You can give it any name you want. Use the IMEI and PIN that you find on the backside of your packaging.</p>
        </div>
        <footer style="text-align: center; margin-top: 10px;">3/${totalSteps}</footer>  <!-- Pagination footer -->
      `,
      buttons: [
        {
          text: 'Previous', // Previous button for going back to the first step
          action: tour.back,
        },
        {
          text: 'Next', // Next button for further steps (if any)
          action: () => {
            tour.next(); // Continue to the next step in the tour
          },
        },
        {
          text: 'Close',
          action: tour.cancel,
        },
      ],
      highlightClass: 'shepherd-highlight', // Custom highlight class
    });
  } else {
    console.warn('tour3 element is not available for the tour.');
  }

  // Start the tour
  tour.start();
}
