## Softwareprojekt Lokalisierung WebApp

#### 1. Working with this repository
#### 2. Installation / Setup
#### 3. Additional information
#### 4. Database information
---

---

---

#### 1. Working with this repository
---

##### GitHub Repository

https://github.com/ngbui101/SWP2425

---

###### Branch Conventions
**Master** is the productive, final branch of the project. Only working and finished code from develop shall be merged.
**Develop** is the one we work on. When a feature or bugfix is done, merge your branch into develop.
**custom** when working on a task, fork from develop and name your new branch like feature/XYZ or bugfix/XYZ. After merging into develop, delete it.

---
#### 2. Installation / Setup
---

#####  Start frontend: 

1. Navigate to folder "frontend"
2. Run "npm install"
3. To start on test server, run "npm run dev"

##### Start backend: 

1. Navigate to folder "backend"
2. Run "npm install"
3. To start the backend, run "npm run dev".

#### Working on MacOS
bcrypt package seems to work different for macOS / windows

by default the settings are configured for windows. when working on macOS, try following: navigate to backend folder, use npm install, then:

1. "npm rebuild bcrypt"
2. "npm uninstall bcrypt"
3. "npm install bcrypt"

**Important:** Until we have a solution, please do **NOT** commit the changes regarding bcrypt - just stage and commit the files you manually changed


---
#### 3. Additional Information
---


---
#### 4. Database Information
---

##### MongoDB database ###


For documentation / how mongoDB works, check out: https://www.mongodb.com/docs/manual/

Database cloud is created at [Atleas MongoDB](https://cloud.mongodb.com/)

You can use the direct link to login and see for yourself [here](https://account.mongodb.com/account/login)

---

Current connection String: mongodb+srv://sop:1234@softwarepraktikum.iporc.mongodb.net/SOP

---
