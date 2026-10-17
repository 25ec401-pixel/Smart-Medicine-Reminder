# Smart Medicine Reminder - Console Version

A text-menu program written in **standard C++17**. No Qt, no SQLite, no CMake,
no external libraries - just the C++ standard library and the GCC compiler that
comes with Code::Blocks.

It is the console edition of the Qt desktop project. The Object-Oriented design
is the same: `Person`, `User`, `Medicine`, `Reminder`, `Tracker`, `Report`.

---

## Features

| Menu option | What it does |
|-------------|--------------|
| 1. Create / edit profile | Saves your name and age (Enter keeps the old value when editing) |
| 2. View profile | Shows the saved profile |
| 3. Add medicine | Name, dosage, daily time (24-hour `HH:MM`), reminder on/off |
| 4. View medicines | Table of all medicines, sorted by time |
| 5. Edit medicine | Change any detail (Enter keeps the old value) |
| 6. Delete medicine | Asks for confirmation; past history is kept |
| 7. Record dose | Mark a dose **Taken** or **Missed** for today or an earlier date |
| 8. Today's schedule | Every medicine with today's status: Taken / Missed / DUE NOW / Pending |
| 9. Intake history | All records, newest first (date, time, medicine, status) |
| 10. Health report | Taken, missed, total, adherence %, a bar, and a per-medicine table |
| 11. Check reminders now | Shows every dose that is due and not yet recorded |
| 12. Clear intake history | Asks for confirmation |
| 13. Help | Explains how reminders work and where the data files are |

**Adherence (%) = (Taken Doses / Total Recorded Doses) x 100**
(shows "N/A" when no dose has been recorded yet).

All input is validated: names (letters only), age (1-120), empty text,
time format, real calendar dates, no future dates, and menu numbers.
Wrong input never crashes the program - it just asks again or shows a message.

---

## IMPORTANT - what is different from the Qt version

| Qt desktop version | Console version |
|--------------------|-----------------|
| Pop-up window appears at the scheduled time (QTimer checks the clock every 10 s) | A console program cannot pop up a window or interrupt typing. Reminders are checked **when the program starts, every time the main menu is shown, and when you choose option 11** |
| Works while the window is minimised | The program must be open (a window you are not typing in does not interrupt you) |
| A dose due within 30 minutes of a missed start is still announced | **Every** dose whose time has passed today and is not recorded is shown as `DUE NOW` |
| Reminders do not work when the app is closed | **Same here: if the program is closed, or the computer is off, no reminder can appear.** Next time you open it, overdue doses are shown immediately |
| SQLite database | Three plain text files |
| Click buttons and tables | Type menu numbers |

A reminder for a dose is shown **once per day** while the program stays open
(so you are not nagged repeatedly). If you choose "Decide later" it stays
pending and the menu shows a one-line note until you record it (option 7 or 11).

Windows can run a program at a set time using *Task Scheduler*, but that is
outside this project - this program itself never runs in the background.

---

## Saved data

Data is saved automatically after every change in three text files, in the
folder the program runs from (its *working folder*):

| File | Content (one record per line, fields separated by a pipe character) |
|------|------------------------------------------------------|
| `smr_profile.txt` | `name\|age` |
| `smr_medicines.txt` | `id\|name\|dosage\|HH:MM\|reminder(1 or 0)` |
| `smr_history.txt` | `medicineId\|name\|dosage\|HH:MM\|date\|recordedAt\|TAKEN or MISSED` |

* If a file does not exist yet (first run) or is empty, the program simply starts with no data.
* If a line is damaged (for example edited by hand), that line is skipped and a note is shown at start-up; the rest of the data still loads.
* The `|` character cannot be used in medicine names or dosages, because it separates the fields.
* If a file cannot be written (read-only folder, full disk) you get a clear "File error" message.
* Run the program from the same folder every time, otherwise it will look for the files somewhere else and appear empty. Option 13 shows the file names.

Do not put these `smr_*.txt` files on GitHub if they contain your personal data.

---

## Files in this project

```
SmartMedicineReminder_Console/
├── SmartMedicineReminder_Console.cbp   Code::Blocks project file
├── README.md
├── main.cpp            starts the program
├── app.h / app.cpp     the text menu (ConsoleApp)
├── person.h / .cpp     abstract base class
├── user.h / .cpp       user profile (inherits Person)
├── medicine.h / .cpp   medicine details
├── reminder.h / .cpp   reminder logic (inherits Medicine)
├── medicinemanager.h / .cpp   add / edit / delete medicines
├── tracker.h / .cpp    records Taken / Missed doses
├── report.h / .cpp     adherence calculation and report data
├── datastore.h / .cpp  ALL file reading and writing
├── intakerecord.h      IntakeStatus enum + IntakeRecord struct
├── exceptions.h        ValidationException, FileException
└── utils.h / .cpp      input, time/date and text helpers
```

---

## How to build and run in Code::Blocks (Windows)

Use the **Code::Blocks version that includes MinGW** (the installer named
`codeblocks-xx.xxmingw-setup.exe`) so that the GCC compiler is installed.

### Step 0 - turn on C++17 (once)

1. Open Code::Blocks.
2. **Settings -> Compiler...**
3. On the **Compiler settings** tab, **Compiler Flags**, tick
   **"Have g++ follow the C++17 ISO C++ language standard [-std=c++17]"**
   (if you only see C++14 / C++11 options, choose the newest one - see the note below).
4. Click OK.

> The code also compiles as C++11, so an older Code::Blocks still works if you tick the C++11 flag.

### Option A - open the ready-made project (easiest)

1. Unzip `SmartMedicineReminder_Console.zip` somewhere simple, for example `C:\Projects\`.
2. In Code::Blocks: **File -> Open...** and choose `SmartMedicineReminder_Console.cbp`.
3. Press **Build and run** (the gear + green triangle, or `F9`).
4. A black console window opens with the menu. Type a number and press Enter.

### Option B - create the project yourself

1. **File -> New -> Project... -> Console application -> Go**.
2. Choose **C++**, give the project the title `SmartMedicineReminder_Console`, choose a folder, and keep **GNU GCC Compiler**. Finish.
3. Delete the sample `main.cpp` from the project (right-click it in the left panel -> **Remove file from project**).
4. Copy all `.h` and `.cpp` files from this folder into the new project folder.
5. In Code::Blocks: **Project -> Add files...** and select every `.cpp` and `.h` file, then OK (tick both Debug and Release if asked).
6. Make sure Step 0 (C++17) is done, then press `F9`.

### Quick test (2 minutes)

1. Option **1** - create your profile.
2. Option **3** - add *Paracetamol*, *500 mg*, a time that has already passed today (for example `00:00`), reminder **y**.
3. Back at the menu a **MEDICINE REMINDER** box appears - choose **1** (Taken).
4. Option **8** (today's schedule) and option **10** (report) show the result.
5. Choose **0** to exit, start the program again, and use option **4** and **9** - your data is still there.

### Build from a terminal instead (optional)

```
g++ -std=c++17 -Wall -o SmartMedicineReminder_Console *.cpp
SmartMedicineReminder_Console.exe
```

### Troubleshooting

| Problem | Fix |
|---------|-----|
| Strange errors about `nullptr`, `to_string` or `override` | The C++ standard flag is not enabled - do Step 0 |
| "Compiler not found" / `gcc not found` | You installed Code::Blocks without MinGW. Reinstall the "mingw-setup" version |
| Console window closes immediately | Run it with **Build and run** (`F9`) from Code::Blocks, which keeps the window open until a key is pressed |
| Program starts with no data | You ran it from a different folder - look for the `smr_*.txt` files and run from that folder |
| `undefined reference` errors | One of the `.cpp` files was not added to the project (use Option A, or add all files in Option B) |

---

## OOP concepts demonstrated

| Concept | Where |
|---------|-------|
| **Abstraction** | `Person` is abstract: `displayProfile()` and `role()` are pure virtual (`= 0`) |
| **Inheritance** | `User : Person` and `Reminder : Medicine` |
| **Polymorphism** | `displayProfile()` / `role()` overridden in `User`; `Medicine::summary()` is virtual and overridden in `Reminder` |
| **Encapsulation** | Data members are `private`; setters (`setName`, `setAge`, `setDosage`, `setTime`) check the input before storing it |
| **File handling** | `DataStore` is the only class that uses `<fstream>`; all other classes work with objects |
| **Exception handling** | `ValidationException` for bad input, `FileException` for unwritable files; the menu catches both with `try/catch` |
| **Input validation** | `util::readInt`, `readYesNo`, the setters, `isValidTime`, `isValidDate` |
| **Composition** | `ConsoleApp` owns a `User`, `MedicineManager`, `Tracker`, `Report` and `DataStore`; `Tracker` and `MedicineManager` use the `DataStore` |

## License

Educational project. Use and modify freely for learning.
