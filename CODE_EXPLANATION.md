# شرح مشروع نظام معلومات الطلاب (Student Information System)

## 📚 نظرة عامة على المشروع

**المشروع ده عبارة عن:** نظام إدارة معلومات طلاب جامعي متكامل باستخدام C++ و MySQL و WebView

**المكونات الأساسية:**
- **Backend**: C++ بيتعامل مع قاعدة البيانات MySQL
- **Frontend**: HTML/CSS/JavaScript بيتعرض في WebView
- **Database**: MySQL لتخزين البيانات
- **Security**: نظام صلاحيات RBAC (Role-Based Access Control)

**الأدوار في النظام:**
1. **Super Admin** - يقدر يعمل أي حاجة
2. **Student Affairs** - إدارة الطلاب والكورسات
3. **Professor** - الدكاترة يدخلوا الدرجات والحضور
4. **Student** - الطلاب يشوفوا درجاتهم

---

## 🎯 ملف main.cpp - الملف الرئيسي

### 1️⃣ الـ Headers والـ Includes (السطر 1-25)

```cpp
#include "include/Database.h"
#include "include/Student.h"
#include "include/Professor.h"
// ... باقي الـ includes
```

**الشرح:**
- بنستورد كل الـ Classes اللي هنستخدمها (Student, Professor, Course, إلخ)
- `lib/webview.h` → مكتبة عشان نعرض واجهة الـ HTML
- مكتبات C++ القياسية زي `<iostream>`, `<string>`, `<vector>`

**لو سألوك:** إيه فايدة الـ includes دي؟
**الإجابة:** بتجيب تعريفات الـ Classes والـ Functions اللي هنستخدمها في البرنامج

---

### 2️⃣ متغير قاعدة البيانات (السطر 26)

```cpp
Database db;
```

**الشرح:**
- ده متغير **global** من نوع Database
- بيستخدمه كل الـ Functions في البرنامج للاتصال بـ MySQL
- **Global** عشان كل الـ lambda functions في الـ WebView تقدر توصله

**لو سألوك:** ليه عملناه global؟
**الإجابة:** عشان الـ lambda functions (زي cpp_login, cpp_getAllStudents) تقدر تستخدمه بدون ما نمرره كـ parameter

---

### 3️⃣ دالة urlEncodePath (السطر 28-40)

```cpp
static string urlEncodePath(const string& path) {
    string out;
    out.reserve(path.size());
    for (unsigned char c : path) {
        switch (c) {
            case ' ': out += "%20"; break;
            case '#': out += "%23"; break;
            case '%': out += "%25"; break;
            default: out += static_cast<char>(c); break;
        }
    }
    return out;
}
```

**الشرح بالتفصيل:**
- **الهدف:** تحويل المسار (path) لـ URL صالح
- **المشكلة:** المسارات فيها مسافات أو رموز خاصة زي # و %
- **الحل:** نحول المسافة لـ %20 والـ # لـ %23 وهكذا
- `out.reserve(path.size())` → بنحجز مساحة في الذاكرة مقدماً (optimization)
- **static** → الدالة محلية للملف ده فقط

**مثال:**
```
Input:  "C:/Program Files/app.exe"
Output: "C:/Program%20Files/app.exe"
```

**لو سألوك:** ليه بنعمل URL encoding؟
**الإجابة:** عشان الـ WebView يقدر يفتح المسار بشكل صحيح، خصوصاً لو فيه مسافات أو رموز خاصة

---

### 4️⃣ دالة toFileUrl (السطر 42-52)

```cpp
static string toFileUrl(std::filesystem::path path) {
    path = std::filesystem::absolute(path);
    string p = path.generic_string();

    // Windows drive letter needs a leading slash in file URLs: /C:/...
    if (p.size() >= 2 && std::isalpha(static_cast<unsigned char>(p[0])) && p[1] == ':') {
        p = "/" + p;
    }

    return "file://" + urlEncodePath(p);
}
```

**الشرح سطر سطر:**
1. `path = std::filesystem::absolute(path)` → بنحول المسار لـ absolute path (مسار كامل من الـ root)
2. `path.generic_string()` → بنحوله لـ string بصيغة عامة (/ بدل \)
3. الـ if → لو المسار Windows (زي C:/) بنضيف / في الأول
4. النتيجة النهائية → `file:///C:/Users/NITRO/...`

**مثال:**
```
Input:  "web/index.html"
Output: "file:///C:/Users/NITRO/source/repos/C++%20Ass2/web/index.html"
```

**لو سألوك:** ليه بنضيف / قبل C:/?
**الإجابة:** عشان الـ file URL protocol محتاج 3 slashes بعد file: في Windows

---

### 5️⃣ دالة getCurrentSemester (السطر 54-68)

```cpp
static bool getCurrentSemester(Database& db, string& semesterId, int& semesterNumber) {
    try {
        auto result = db.executeQuery(
            "SELECT id, semester_number FROM semesters WHERE is_current = TRUE LIMIT 1"
        );
        if (result->next()) {
            semesterId = result->getString("id");
            semesterNumber = result->getInt("semester_number");
            return true;
        }
    } catch (const exception& e) {
        cerr << "[Main] Error loading current semester: " << e.what() << endl;
    }
    return false;
}
```

**الشرح:**
- **الهدف:** جلب الترم الحالي من قاعدة البيانات
- **Parameters بالـ reference (&):** عشان نرجع القيم من الدالة
- `is_current = TRUE` → عمود في الـ database بيحدد الترم الحالي
- `LIMIT 1` → نجيب ترم واحد بس
- **try-catch** → للتعامل مع الأخطاء

**لو سألوك:** ليه استخدمنا reference parameters؟
**الإجابة:** عشان نرجع أكتر من قيمة واحدة من الدالة (semesterId و semesterNumber)

---

### 6️⃣ دالة main - البداية (السطر 70-83)

```cpp
int main() {
    cout << "========================================" << endl;
    cout << "   Student Information System v2.0     " << endl;
    cout << "   NCTU - With RBAC                    " << endl;
    cout << "========================================" << endl;

    if (!db.testConnection()) {
        cerr << "ERROR: Cannot connect to database!" << endl;
        cerr << "Check your MySQL password in Database.h" << endl;
        cerr << "Press Enter to exit..." << endl;
        cin.get();
        return 1;
    }
```

**الشرح:**
1. بنطبع رسالة ترحيبية
2. `db.testConnection()` → بنتأكد إننا متصلين بالـ database
3. لو فشل الاتصال → بنطبع رسالة خطأ ونخرج من البرنامج
4. `return 1` → معناها البرنامج خرج بخطأ (0 = نجاح)

**لو سألوك:** ليه بنختبر الاتصال في البداية؟
**الإجابة:** عشان لو مفيش اتصال بالـ database، مفيش فايدة من تشغيل البرنامج

---

### 7️⃣ إنشاء نافذة WebView (السطر 85-89)

```cpp
webview::webview w(true, nullptr);
w.set_title("NCTU - Student Information System");
w.set_size(1280, 800, WEBVIEW_HINT_NONE);

cout << "[Main] WebView window created" << endl;
```

**الشرح:**
- `webview::webview w(true, nullptr)` → إنشاء نافذة WebView
  - **true** → debug mode مفعل
  - **nullptr** → default settings
- `set_title()` → عنوان النافذة
- `set_size(1280, 800)` → حجم النافذة (عرض × ارتفاع)
- `WEBVIEW_HINT_NONE` → النافذة عادية (مش full screen ولا minimized)

**لو سألوك:** إيه هو WebView؟
**الإجابة:** مكتبة بتعرض صفحات HTML داخل برنامج C++، زي browser صغير

---

### 8️⃣ Authentication - دالة cpp_login (السطر 94-134)

```cpp
w.bind("cpp_login", [](const string& request) -> string {
    try {
        string username = JsonHelper::parseSimpleValue(request, "username");
        string password = JsonHelper::parseSimpleValue(request, "password");

        cout << "[Login] Attempting login: " << username << endl;

        if (SessionManager::login(db, username, password)) {
            const UserSession& session = SessionManager::getSession();
            
            vector<pair<string, string>> response;
            response.push_back({"success", JsonHelper::makeBool(true)});
            response.push_back({"role", JsonHelper::makeString(session.roleId)});
            // ... باقي البيانات
            
            SessionManager::logAction(db, "LOGIN", "users", session.userId, "Successful login");
            return JsonHelper::makeObject(response);
        } else {
            // فشل تسجيل الدخول
            return JsonHelper::errorResponse("Invalid credentials");
        }
    } catch (const exception& e) {
        return JsonHelper::errorResponse(e.what());
    }
});
```

**الشرح التفصيلي:**

**1. w.bind("cpp_login", ...):**
- بنربط function في C++ باسم "cpp_login"
- الـ JavaScript في الـ frontend يقدر ينادي على الدالة دي
- **Lambda function** → دالة بدون اسم `[](parameters) -> return_type { }`

**2. استخراج البيانات:**
```cpp
string username = JsonHelper::parseSimpleValue(request, "username");
```
- الـ request جاي من JavaScript بصيغة JSON
- بنستخرج username و password منه

**3. عملية تسجيل الدخول:**
```cpp
if (SessionManager::login(db, username, password)) {
```
- بنستدعي SessionManager اللي بيتحقق من username و password
- لو صح → بيرجع true ويخزن بيانات المستخدم في session
- لو غلط → بيرجع false

**4. إرجاع البيانات:**
```cpp
const UserSession& session = SessionManager::getSession();
response.push_back({"role", JsonHelper::makeString(session.roleId)});
```
- بنجيب بيانات الـ session
- بنبني response object فيه:
  - success: true
  - role: الدور (student, professor, admin)
  - permissions: الصلاحيات
  - name, id, إلخ

**5. Audit Log:**
```cpp
SessionManager::logAction(db, "LOGIN", "users", session.userId, "Successful login");
```
- بنسجل عملية تسجيل الدخول في audit_log table
- مهم للأمان وتتبع الإجراءات

**لو سألوك:** إيه الفرق بين bind و function عادية؟
**الإجابة:** bind بتربط function C++ بـ JavaScript عشان تقدر تناديها من الـ frontend

**لو سألوك:** ليه بنستخدم lambda function؟
**الإجابة:** عشان الكود يبقى مباشر ومختصر، والـ lambda تقدر تستخدم المتغيرات الـ global زي db

---

### 9️⃣ دالة cpp_logout (السطر 136-139)

```cpp
w.bind("cpp_logout", [](const string& request) -> string {
    SessionManager::logout();
    return JsonHelper::successResponse("Logged out");
});
```

**الشرح:**
- بنمسح بيانات الـ session الحالية
- `SessionManager::logout()` → بيعيد تعيين currentSession لقيم فاضية
- بنرجع رسالة نجاح

**لو سألوك:** ليه مهم نعمل logout function؟
**الإجابة:** عشان ننضف الـ session ونمنع أي حد تاني يستخدم البرنامج بدون ما يسجل دخول

---

### 🔟 دالة cpp_getSession (السطر 141-154)

```cpp
w.bind("cpp_getSession", [](const string& request) -> string {
    if (!SessionManager::isLoggedIn()) {
        return JsonHelper::errorResponse("Not logged in");
    }
    const UserSession& session = SessionManager::getSession();
    vector<pair<string, string>> response;
    response.push_back({"userId", JsonHelper::makeString(session.userId)});
    response.push_back({"username", JsonHelper::makeString(session.username)});
    // ... باقي البيانات
    return JsonHelper::makeObject(response);
});
```

**الشرح:**
- بتجيب بيانات المستخدم الحالي
- بنتحقق الأول إنه مسجل دخول
- بنرجع بياناته (userId, username, fullName, role, linkedId)

**لو سألوك:** ليه بنتحقق من isLoggedIn؟
**الإجابة:** للأمان، عشان نتأكد إن في مستخدم مسجل دخول قبل ما نرجع بياناته

---

### 1️⃣1️⃣ Dashboard Statistics (السطر 159-200)

```cpp
w.bind("cpp_getDashboardStats", [](const string& request) -> string {
    try {
        vector<pair<string, string>> stats;
        
        if (SessionManager::canViewAllStudents()) {
            // Admin يشوف إحصائيات عامة
            auto result = db.executeQuery("SELECT COUNT(*) as count FROM students");
            if (result->next()) 
                stats.push_back({"students", JsonHelper::makeNumber(result->getInt("count"))});
            
            // نفس الشيء للأساتذة، الكورسات، والأقسام
        } else if (SessionManager::getSession().isProfessor()) {
            // Professor يشوف الكورسات المسندة له فقط
            auto courseIds = SessionManager::getAssignedCourseIds(db, semesterId);
            stats.push_back({"assignedCourses", JsonHelper::makeNumber((int)courseIds.size())});
            
            // عدد الطلاب في كورساته
            int totalStudents = 0;
            for (const auto& cid : courseIds) {
                // نعد طلاب كل كورس
            }
        }
        return JsonHelper::makeObject(stats);
    } catch (const exception& e) {
        return JsonHelper::errorResponse(e.what());
    }
});
```

**الشرح:**
- **Permission-based:** كل مستخدم يشوف إحصائيات مختلفة حسب دوره
- **Admin:** يشوف عدد الطلاب، الأساتذة، الكورسات، الأقسام
- **Professor:** يشوف الكورسات المسندة له وعدد طلابها
- **Student:** مش بيشوف إحصائيات في الـ dashboard

**لو سألوك:** ليه مش كل الأدوار بتشوف نفس الإحصائيات؟
**الإجابة:** عشان الـ Role-Based Access Control (RBAC) - كل واحد يشوف المعلومات اللي محتاجها بس

---

### 1️⃣2️⃣ Students Management - cpp_getAllStudents (السطر 205-252)

```cpp
w.bind("cpp_getAllStudents", [](const string& request) -> string {
    try {
        if (!SessionManager::canViewAllStudents()) {
            return "[]";  // مصفوفة فاضية لو مفيش صلاحية
        }
        
        // نتحقق من صلاحية عرض الباسوورد
        bool canViewPasswords = SessionManager::hasPermission(Permissions::PWD_VIEW_ALL) ||
                               SessionManager::hasPermission(Permissions::PWD_VIEW_STU);
        
        // بناء الـ query
        string query = 
            "SELECT s.id, s.first_name, s.last_name, s.date_of_birth, s.gender, "
            "u.email, u.phone, s.enrollment_date, s.academic_level_id, s.department_id";
        if (canViewPasswords) {
            query += ", u.password_hash";  // نضيف الباسوورد لو فيه صلاحية
        }
        query += " FROM students s LEFT JOIN users u ON s.user_id = u.id ORDER BY s.id";
        
        auto result = db.executeQuery(query);
        vector<string> jsonStudents;
        
        while (result->next()) {
            // بناء JSON object لكل طالب
            stringstream ss;
            ss << "{";
            ss << "\"id\":\"" << result->getString("id") << "\",";
            ss << "\"firstName\":\"" << result->getString("first_name") << "\",";
            // ... باقي البيانات
            if (canViewPasswords) {
                ss << ",\"password\":\"" << result->getString("password_hash") << "\"";
            }
            ss << "}";
            jsonStudents.push_back(ss.str());
        }
        return JsonHelper::makeArray(jsonStudents);
    } catch (const exception& e) {
        return "[]";
    }
});
```

**الشرح التفصيلي:**

**1. التحقق من الصلاحيات:**
```cpp
if (!SessionManager::canViewAllStudents()) {
    return "[]";
}
```
- لو المستخدم مالوش صلاحية، نرجع مصفوفة فاضية

**2. Permission-based fields:**
```cpp
bool canViewPasswords = SessionManager::hasPermission(Permissions::PWD_VIEW_ALL) ||
                       SessionManager::hasPermission(Permissions::PWD_VIEW_STU);
```
- بنتحقق من صلاحية عرض الباسوورد
- بس Super Admin عنده الصلاحية دي

**3. SQL JOIN:**
```cpp
"FROM students s LEFT JOIN users u ON s.user_id = u.id"
```
- بنجيب بيانات الطالب من جدول students
- و email و phone من جدول users
- **LEFT JOIN** عشان لو الطالب مالوش user account، يظهر برضو

**4. بناء JSON response:**
```cpp
stringstream ss;
ss << "{";
ss << "\"id\":\"" << result->getString("id") << "\",";
```
- بنبني JSON object لكل طالب يدوياً
- بنضيفه للـ vector
- في الآخر بنحول الـ vector لـ JSON array

**لو سألوك:** ليه بنستخدم LEFT JOIN مش INNER JOIN؟
**الإجابة:** عشان لو الطالب مالوش user account (user_id = NULL)، يظهر برضو في النتائج

**لو سألوك:** ليه بنبني JSON يدوياً مش بنستخدم library؟
**الإجابة:** عشان المشروع بسيط ومش محتاج library كبيرة زي nlohmann/json، ده أخف وأسرع

---

### 1️⃣3️⃣ cpp_addStudent - إضافة طالب (السطر 254-333)

```cpp
w.bind("cpp_addStudent", [](const string& request) -> string {
    try {
        // 1. التحقق من الصلاحية
        if (!SessionManager::hasPermission(Permissions::STU_CREATE)) {
            return JsonHelper::errorResponse("Permission denied");
        }
        
        // 2. استخراج البيانات من request
        string id = JsonHelper::parseSimpleValue(request, "id");
        string firstName = JsonHelper::parseSimpleValue(request, "firstName");
        // ... باقي البيانات
        
        // 3. التحقق من عدم وجود الطالب
        if (Student::exists(db, id)) {
            return JsonHelper::errorResponse("Student ID already exists!");
        }
        
        // 4. تحويل gender لحروف صغيرة (عشان enum في MySQL)
        transform(gender.begin(), gender.end(), gender.begin(),
                  [](unsigned char c) { return static_cast<char>(tolower(c)); });
        
        // 5. إنشاء object Student
        Student student(id, firstName, lastName, dateOfBirth, gender,
                       email, phone, academicLevelId, departmentId);
        
        // 6. حفظ الطالب
        if (student.save(db)) {
            // 7. تسجيل تلقائي في كورسات الترم الحالي
            int registeredCount = 0;
            string semesterId;
            int semesterNumber = 0;
            if (getCurrentSemester(db, semesterId, semesterNumber)) {
                try {
                    db.executeUpdate("START TRANSACTION");
                    
                    // جلب كورسات القسم والمستوى والترم
                    auto coursesStmt = db.prepareStatement(
                        "SELECT id FROM courses "
                        "WHERE department_id = ? AND academic_level_id = ? AND semester_number = ?"
                    );
                    coursesStmt->setString(1, departmentId);
                    coursesStmt->setString(2, academicLevelId);
                    coursesStmt->setInt(3, semesterNumber);
                    auto coursesRes = coursesStmt->executeQuery();
                    
                    while (coursesRes->next()) {
                        string courseId = coursesRes->getString("id");
                        
                        // تسجيل الطالب في الكورس
                        auto regStmt = db.prepareStatement(
                            "INSERT INTO registrations (student_id, course_id, semester_id, status, registered_by) "
                            "VALUES (?, ?, ?, 'registered', ?)"
                        );
                        regStmt->setString(1, id);
                        regStmt->setString(2, courseId);
                        regStmt->setString(3, semesterId);
                        regStmt->setString(4, SessionManager::getSession().userId);
                        regStmt->executeUpdate();
                        
                        // إنشاء سجل درجات فارغ
                        string regId = to_string(db.getLastInsertId());
                        auto gradeStmt = db.prepareStatement(
                            "INSERT INTO grades (registration_id) VALUES (?)"
                        );
                        gradeStmt->setString(1, regId);
                        gradeStmt->executeUpdate();
                        
                        registeredCount++;
                    }
                    db.executeUpdate("COMMIT");
                } catch (const exception& e) {
                    try { db.executeUpdate("ROLLBACK"); } catch (...) {}
                }
            }
            
            // 8. تسجيل في audit log
            SessionManager::logAction(db, "CREATE", "students", id,
                                      "Added student and registered " + to_string(registeredCount) + " courses");
            return JsonHelper::successResponse("Student added successfully!");
        }
        return JsonHelper::errorResponse("Failed to add student");
    } catch (const exception& e) {
        return JsonHelper::errorResponse(e.what());
    }
});
```

**الشرح المفصل:**

**خطوات الدالة:**

**1. Permission Check:**
```cpp
if (!SessionManager::hasPermission(Permissions::STU_CREATE)) {
    return JsonHelper::errorResponse("Permission denied");
}
```
- **ليه؟** عشان بس اللي عنده صلاحية يقدر يضيف طلاب

**2. Data Extraction:**
```cpp
string id = JsonHelper::parseSimpleValue(request, "id");
```
- بنستخرج كل البيانات من JSON request

**3. Validation:**
```cpp
if (Student::exists(db, id)) {
    return JsonHelper::errorResponse("Student ID already exists!");
}
```
- بنتأكد إن الـ ID مش موجود قبل كده

**4. Gender Normalization:**
```cpp
transform(gender.begin(), gender.end(), gender.begin(),
          [](unsigned char c) { return static_cast<char>(tolower(c)); });
```
- بنحول gender لـ lowercase
- **ليه؟** عشان في MySQL الـ enum عايزه قيم محددة ('male', 'female')

**5. Auto-Registration:**
```cpp
if (getCurrentSemester(db, semesterId, semesterNumber)) {
```
- بنسجل الطالب تلقائياً في كورسات قسمه ومستواه في الترم الحالي
- **TRANSACTION:** عشان لو حصل خطأ في النص، نرجع كل الحاجات زي ما كانت

**6. Grade Record Creation:**
```cpp
auto gradeStmt = db.prepareStatement(
    "INSERT INTO grades (registration_id) VALUES (?)"
);
```
- لكل تسجيل في كورس، بنعمل سجل درجات فارغ
- عشان الدكتور بعدين يدخل الدرجات

**7. Audit Logging:**
```cpp
SessionManager::logAction(db, "CREATE", "students", id, "Added student and registered " + to_string(registeredCount) + " courses");
```
- بنسجل العملية في audit_log
- مهم للتتبع والأمان

**لو سألوك:** ليه بنستخدم TRANSACTION؟
**الإجابة:** عشان نتأكد إن كل العمليات (تسجيل في كل الكورسات) تتم بنجاح، أو متتمش خالص. مش نص نص.

**لو سألوك:** ليه بنعمل auto-registration؟
**الإجابة:** عشان نوفر وقت، بدل ما الـ admin يسجل الطالب يدوياً في كل كورس

---

### 1️⃣4️⃣ cpp_updateStudent (السطر 335-365)

```cpp
w.bind("cpp_updateStudent", [](const string& request) -> string {
    try {
        if (!SessionManager::hasPermission(Permissions::STU_EDIT)) {
            return JsonHelper::errorResponse("Permission denied");
        }
        
        // استخراج البيانات
        string id = JsonHelper::parseSimpleValue(request, "id");
        // ... باقي البيانات
        
        // تحويل gender لـ lowercase
        transform(gender.begin(), gender.end(), gender.begin(),
                  [](unsigned char c) { return static_cast<char>(tolower(c)); });
        
        // إنشاء object وتحديث
        Student student(id, firstName, lastName, dateOfBirth, gender,
                       email, phone, academicLevelId, departmentId);
        
        if (student.update(db)) {
            SessionManager::logAction(db, "UPDATE", "students", id, "Updated student");
            return JsonHelper::successResponse("Student updated successfully!");
        }
        return JsonHelper::errorResponse("Failed to update student");
    } catch (const exception& e) {
        return JsonHelper::errorResponse(e.what());
    }
});
```

**الشرح:**
- شبيه addStudent بس بيستدعي `student.update(db)` مش `save()`
- `update()` بيعمل UPDATE في الـ database بدل INSERT
- بيحدث بيانات الطالب في جدول students وجدول users

**لو سألوك:** إيه الفرق بين save و update؟
**الإجابة:** 
- **save** → INSERT (إضافة سجل جديد)
- **update** → UPDATE (تعديل سجل موجود)

---

### 1️⃣5️⃣ cpp_deleteStudent (السطر 367-387)

```cpp
w.bind("cpp_deleteStudent", [](const string& request) -> string {
    try {
        if (!SessionManager::hasPermission(Permissions::STU_DELETE)) {
            return JsonHelper::errorResponse("Permission denied");
        }
        
        // استخراج ID من request
        string id = request;
        id.erase(remove(id.begin(), id.end(), '['), id.end());
        id.erase(remove(id.begin(), id.end(), ']'), id.end());
        id.erase(remove(id.begin(), id.end(), '"'), id.end());
        
        Student student = Student::getById(db, id);
        if (student.remove(db)) {
            SessionManager::logAction(db, "DELETE", "students", id, "Deleted student");
            return JsonHelper::successResponse("Student deleted!");
        }
        return JsonHelper::errorResponse("Failed to delete student");
    } catch (const exception& e) {
        return JsonHelper::errorResponse(e.what());
    }
});
```

**الشرح:**

**1. تنظيف الـ ID:**
```cpp
id.erase(remove(id.begin(), id.end(), '['), id.end());
id.erase(remove(id.begin(), id.end(), ']'), id.end());
id.erase(remove(id.begin(), id.end(), '"'), id.end());
```
- الـ request ممكن ييجي بصيغة `["20241"]`
- بنشيل الأقواس والـ quotes عشان نطلع الـ ID النظيف

**2. Cascading Delete:**
```cpp
if (student.remove(db)) {
```
- `remove()` في Student class بتمسح:
  - السجل من students table
  - السجل من users table
  - كل التسجيلات registrations (على حسب الـ foreign key constraints)
  - كل الدرجات grades

**لو سألوك:** إيه المشكلة لو محذفناش الأقواس من الـ ID؟
**الإجابة:** الـ query هيفشل عشان هيدور على ID اسمه ["20241"] بدل 20241

---

### 1️⃣6️⃣ cpp_generateStudentId (السطر 389-398)

```cpp
w.bind("cpp_generateStudentId", [](const string& request) -> string {
    try {
        string newId = Student::generateNextId(db);
        vector<pair<string, string>> result;
        result.push_back({"id", JsonHelper::makeString(newId)});
        return JsonHelper::makeObject(result);
    } catch (const exception& e) {
        return JsonHelper::errorResponse(e.what());
    }
});
```

**الشرح:**
- بتولد ID تلقائي للطالب الجديد
- الصيغة: السنة الحالية + رقم تسلسلي (مثال: 20241, 20242, 20243)
- `Student::generateNextId(db)` بتشوف آخر ID موجود وتزود عليه 1

**لو سألوك:** ليه مش بنستخدم AUTO_INCREMENT؟
**الإجابة:** عشان عايزين format معين (السنة + رقم) مش رقم عشوائي

---

## 🎓 شرح الـ Classes الأساسية

### 📊 Database Class

**الملفات:**
- `include/Database.h` (التعريف والتنفيذ)
- `src/Database.cpp` (ملف فارغ للتوافقية)

**المكونات الأساسية:**

#### 1. ResultSet Class

```cpp
class ResultSet {
private:
    MYSQL_RES* result;       // مؤشر على نتيجة الاستعلام
    MYSQL_ROW currentRow;    // السطر الحالي
    MYSQL_FIELD* fields;     // أسماء الأعمدة
    unsigned int numFields;  // عدد الأعمدة
```

**الغرض:** تغليف نتائج الاستعلام من MySQL

**الدوال المهمة:**
- `bool next()` → الانتقال للسطر التالي (زي iterator)
- `getString(fieldName)` → جلب قيمة نصية
- `getInt(fieldName)` → جلب قيمة رقمية صحيحة
- `getDouble(fieldName)` → جلب قيمة عشرية
- `getBoolean(fieldName)` → جلب قيمة منطقية

**مثال استخدام:**
```cpp
auto result = db.executeQuery("SELECT * FROM students");
while (result->next()) {
    string id = result->getString("id");
    string name = result->getString("first_name");
    cout << id << ": " << name << endl;
}
```

**لو سألوك:** ليه عملنا ResultSet class بدل ما نستخدم MYSQL_RES مباشرة؟
**الإجابة:** عشان نخفي تفاصيل MySQL ونخلي الكود أسهل وأنضف (Abstraction)

---

#### 2. PreparedStatement Class

```cpp
class PreparedStatement {
private:
    MYSQL* conn;               // الاتصال بقاعدة البيانات
    string query;              // الاستعلام (فيه ?)
    vector<string> params;     // القيم اللي هنحطها مكان ?
    int paramCount;            // عدد الـ ?
```

**الغرض:** تنفيذ Prepared Statements (حماية من SQL Injection)

**الدوال المهمة:**
- `setString(index, value)` → وضع قيمة نصية
- `setInt(index, value)` → وضع قيمة رقمية
- `setDouble(index, value)` → وضع قيمة عشرية
- `setNull(index)` → وضع NULL
- `executeQuery()` → تنفيذ SELECT
- `executeUpdate()` → تنفيذ INSERT/UPDATE/DELETE

**مثال:**
```cpp
auto stmt = db.prepareStatement("SELECT * FROM students WHERE id = ?");
stmt->setString(1, "20241");  // المعامل الأول
auto result = stmt->executeQuery();
```

**كيف تعمل:**
```cpp
string buildQuery() const {
    string result;
    int paramIdx = 0;
    for (char c : query) {
        if (c == '?' && paramIdx < paramCount) {
            result += params[paramIdx++];  // نستبدل ? بالقيمة
        } else {
            result += c;
        }
    }
    return result;
}
```

**لو سألوك:** ليه بنستخدم Prepared Statements؟
**الإجابة:** 
1. **حماية من SQL Injection** → القيم بتتعامل صح
2. **أداء أفضل** → MySQL بتعمل optimize للـ query
3. **كود أنضف** → مفيش concatenation معقد

---

#### 3. Database Class

**المتغيرات:**
```cpp
MYSQL* connection;     // الاتصال بالـ MySQL
string host;           // عنوان السيرفر (localhost)
string user;           // اسم المستخدم (root)
string password;       // كلمة المرور (admin)
string database;       // اسم قاعدة البيانات (nctu_sis)
int port;              // منفذ الاتصال (3306)
```

**Constructor:**
```cpp
Database() 
    : connection(nullptr),
      host("localhost"),
      user("root"),
      password("admin"),      // ⚠️ غير الباسوورد هنا
      database("nctu_sis"),
      port(3306) {
}
```

**الدوال المهمة:**

**1. connect() - الاتصال:**
```cpp
bool connect() {
    if (connection) return true;  // لو متصلين خلاص
    
    connection = mysql_init(nullptr);  // تهيئة
    if (!connection) return false;
    
    if (!mysql_real_connect(connection, host.c_str(), user.c_str(), 
                            password.c_str(), database.c_str(), 
                            port, nullptr, 0)) {
        // فشل الاتصال
        return false;
    }
    
    mysql_set_character_set(connection, "utf8mb4");  // دعم العربي
    return true;
}
```

**2. executeQuery() - تنفيذ SELECT:**
```cpp
unique_ptr<ResultSet> executeQuery(const string& query) {
    if (!connect()) throw runtime_error("Not connected");
    
    if (mysql_query(connection, query.c_str()) != 0) {
        throw runtime_error(string("Query error: ") + mysql_error(connection));
    }
    
    MYSQL_RES* res = mysql_store_result(connection);
    return make_unique<ResultSet>(res);
}
```

**3. executeUpdate() - تنفيذ INSERT/UPDATE/DELETE:**
```cpp
int executeUpdate(const string& query) {
    if (!connect()) throw runtime_error("Not connected");
    
    if (mysql_query(connection, query.c_str()) != 0) {
        throw runtime_error(mysql_error(connection));
    }
    
    return (int)mysql_affected_rows(connection);  // عدد الصفوف المتأثرة
}
```

**لو سألوك:** ليه بنستخدم unique_ptr؟
**الإجابة:** عشان الذاكرة تتحرر أوتوماتيكياً بعد استخدام ResultSet (Smart Pointer)

---

### 👨‍🎓 Student Class

**الملفات:**
- `include/Student.h` (التعريف)
- `src/Student.cpp` (التنفيذ)

**المتغيرات (Attributes):**
```cpp
string id;              // رقم الطالب (مثال: 20241)
string userId;          // رقم المستخدم في جدول users
string firstName;       // الاسم الأول
string lastName;        // اسم العائلة
string dateOfBirth;     // تاريخ الميلاد
string gender;          // الجنس (male/female)
string email;           // البريد الإلكتروني
string phone;           // رقم الهاتف
string registrationDate;// تاريخ التسجيل
string academicLevelId; // المستوى الدراسي (LEVEL1, LEVEL2, ...)
string departmentId;    // القسم (IT, CS, IS, ...)
string passwordHash;    // كلمة المرور (افتراضياً: student123)
```

**الدوال الثابتة (Static Methods):**

**1. getAllStudents():**
```cpp
static vector<Student> getAllStudents(Database& db) {
    vector<Student> students;
    
    auto result = db.executeQuery(
        "SELECT s.id, s.user_id, s.first_name, s.last_name, ... "
        "FROM students s "
        "LEFT JOIN users u ON s.user_id = u.id "
        "ORDER BY s.id"
    );
    
    while (result->next()) {
        Student student;
        student.id = result->getString("id");
        student.firstName = result->getString("first_name");
        // ... باقي البيانات
        students.push_back(student);
    }
    
    return students;
}
```

**ليه static؟** عشان نقدر نناديها بدون إنشاء object: `Student::getAllStudents(db)`

**2. exists():**
```cpp
static bool exists(Database& db, const string& id) {
    auto stmt = db.prepareStatement("SELECT COUNT(*) as count FROM students WHERE id = ?");
    stmt->setString(1, id);
    auto result = stmt->executeQuery();
    
    if (result->next()) {
        return result->getInt("count") > 0;
    }
    return false;
}
```

**3. generateNextId():**
```cpp
static string generateNextId(Database& db) {
    // جلب السنة الحالية
    string yearStr = to_string(getCurrentYear());  // مثال: "2024"
    
    // البحث عن آخر ID بنفس السنة
    auto stmt = db.prepareStatement(
        "SELECT MAX(CAST(id AS UNSIGNED)) as max_id FROM students WHERE id LIKE ?"
    );
    stmt->setString(1, yearStr + "%");  // "2024%"
    auto result = stmt->executeQuery();
    
    int nextNumber = 1;
    if (result->next()) {
        string maxId = result->getString("max_id");
        if (!maxId.empty() && maxId != "0") {
            int lastNumber = stoi(maxId.substr(4));  // استخراج الرقم بعد السنة
            nextNumber = lastNumber + 1;
        }
    }
    
    return yearStr + to_string(nextNumber);  // "20241", "20242", ...
}
```

**مثال:** لو آخر ID كان 20245، التالي يكون 20246

**الدوال العادية (Instance Methods):**

**1. save():**
```cpp
bool save(Database& db) {
    if (exists(db, this->id)) return false;  // الـ ID موجود
    
    try {
        db.executeUpdate("START TRANSACTION");
        
        // إنشاء user account
        auto userStmt = db.prepareStatement(
            "INSERT INTO users (username, password_hash, full_name, email, phone, role_id) "
            "VALUES (?, ?, ?, ?, ?, 'ROLE_STUDENT')"
        );
        userStmt->setString(1, id);
        userStmt->setString(2, passwordHash);
        userStmt->setString(3, getFullName());
        // ... باقي البيانات
        userStmt->executeUpdate();
        
        userId = to_string(db.getLastInsertId());  // جلب ID المستخدم
        
        // إنشاء student profile
        auto stmt = db.prepareStatement(
            "INSERT INTO students (id, user_id, first_name, last_name, ...) "
            "VALUES (?, ?, ?, ?, ...)"
        );
        stmt->setString(1, id);
        stmt->setString(2, userId);
        // ... باقي البيانات
        stmt->executeUpdate();
        
        db.executeUpdate("COMMIT");
        return true;
    } catch (exception& e) {
        db.executeUpdate("ROLLBACK");
        return false;
    }
}
```

**الخطوات:**
1. التحقق من عدم وجود ID
2. بداية transaction
3. إنشاء سجل في users
4. إنشاء سجل في students
5. commit

**2. update():**
```cpp
bool update(Database& db) {
    try {
        db.executeUpdate("START TRANSACTION");
        
        // تحديث students table
        auto stmt = db.prepareStatement(
            "UPDATE students SET first_name = ?, last_name = ?, ... WHERE id = ?"
        );
        // ... تعيين القيم
        stmt->executeUpdate();
        
        // تحديث users table
        auto userStmt = db.prepareStatement(
            "UPDATE users SET full_name = ?, email = ?, phone = ? WHERE id = ?"
        );
        // ... تعيين القيم
        userStmt->executeUpdate();
        
        db.executeUpdate("COMMIT");
        return true;
    } catch (exception& e) {
        db.executeUpdate("ROLLBACK");
        return false;
    }
}
```

**3. remove():**
```cpp
bool remove(Database& db) {
    try {
        db.executeUpdate("START TRANSACTION");
        
        // حذف من students
        auto stmt = db.prepareStatement("DELETE FROM students WHERE id = ?");
        stmt->setString(1, id);
        int rowsAffected = stmt->executeUpdate();
        
        // حذف من users
        if (rowsAffected > 0 && !userId.empty()) {
            auto userStmt = db.prepareStatement("DELETE FROM users WHERE id = ?");
            userStmt->setString(1, userId);
            userStmt->executeUpdate();
        }
        
        db.executeUpdate("COMMIT");
        return rowsAffected > 0;
    } catch (exception& e) {
        db.executeUpdate("ROLLBACK");
        return false;
    }
}
```

**لو سألوك:** ليه بنحذف من students الأول؟
**الإجابة:** عشان students فيها foreign key على users، لازم نحذف الـ child الأول

---

### 📝 Grade Class

**المتغيرات:**
```cpp
string id;              // رقم سجل الدرجة
string registrationId;  // رقم التسجيل
double assignment1;     // درجة الواجب الأول
double assignment2;     // درجة الواجب الثاني
double yearWork;        // أعمال السنة
double finalExam;       // الامتحان النهائي
double total;           // المجموع
double percentage;      // النسبة المئوية
double gpa;             // نقاط التقدير (0-4)
string evaluation;      // التقدير (Excellent, Very Good, ...)
string letterGrade;     // الحرف (A, B, C, D, F)
```

**الدوال المهمة:**

**1. getDistributionForRegistration():**
```cpp
static GradeDistribution getDistributionForRegistration(Database& db, const string& registrationId) {
    GradeDistribution dist{0.0, 0.0, 0.0, 0.0, 0.0};
    
    auto stmt = db.prepareStatement(
        "SELECT c.max_marks, c.assignment1_marks, c.assignment2_marks, "
        "c.year_work_marks, c.final_exam_marks "
        "FROM courses c "
        "JOIN registrations r ON r.course_id = c.id "
        "WHERE r.id = ?"
    );
    stmt->setString(1, registrationId);
    auto result = stmt->executeQuery();
    
    if (result->next()) {
        dist.assignment1Max = result->getDouble("assignment1_marks");
        dist.assignment2Max = result->getDouble("assignment2_marks");
        dist.yearWorkMax = result->getDouble("year_work_marks");
        dist.finalExamMax = result->getDouble("final_exam_marks");
        dist.totalMax = result->getDouble("max_marks");
    }
    
    return dist;
}
```

**الغرض:** جلب توزيع الدرجات للكورس

**2. validateGradeComponent():**
```cpp
static string validateGradeComponent(Database& db, const string& registrationId,
                                     const string& component, double value) {
    GradeDistribution dist = getDistributionForRegistration(db, registrationId);
    
    double maxValue = 0;
    if (component == "assignment1") maxValue = dist.assignment1Max;
    else if (component == "assignment2") maxValue = dist.assignment2Max;
    else if (component == "year_work") maxValue = dist.yearWorkMax;
    else if (component == "final_exam") maxValue = dist.finalExamMax;
    
    if (value < 0) return "Cannot be negative";
    if (value > maxValue) return "Cannot exceed " + to_string(maxValue);
    
    return "";  // فارغ = valid
}
```

**الغرض:** التحقق من صحة الدرجة قبل الحفظ

**3. calculateGrade():**
```cpp
static bool calculateGrade(Database& db, const string& registrationId) {
    // جلب الدرجات الحالية
    auto stmt = db.prepareStatement(
        "SELECT g.assignment1, g.assignment2, g.year_work, g.final_exam, c.max_marks "
        "FROM grades g "
        "JOIN registrations r ON g.registration_id = r.id "
        "JOIN courses c ON r.course_id = c.id "
        "WHERE g.registration_id = ?"
    );
    stmt->setString(1, registrationId);
    auto result = stmt->executeQuery();
    
    if (!result->next()) return false;
    
    double assignment1 = result->getDouble("assignment1");
    double assignment2 = result->getDouble("assignment2");
    double yearWork = result->getDouble("year_work");
    double finalExam = result->getDouble("final_exam");
    double maxMarks = result->getDouble("max_marks");
    
    // حساب المجموع والنسبة
    double total = assignment1 + assignment2 + yearWork + finalExam;
    double percentage = (maxMarks > 0) ? (total / maxMarks * 100.0) : 0.0;
    
    // تحديد التقدير والـ GPA
    string evaluation = getEvaluationFromPercentage(percentage);
    double gpa = getGpaFromPercentage(percentage);
    string letter = getLetterGrade(percentage);
    
    // تحديث قاعدة البيانات
    auto updateStmt = db.prepareStatement(
        "UPDATE grades SET total_marks = ?, percentage = ?, gpa = ?, "
        "evaluation = ?, letter_grade = ? WHERE registration_id = ?"
    );
    updateStmt->setDouble(1, total);
    updateStmt->setDouble(2, percentage);
    updateStmt->setDouble(3, gpa);
    updateStmt->setString(4, evaluation);
    updateStmt->setString(5, letter);
    updateStmt->setString(6, registrationId);
    updateStmt->executeUpdate();
    
    return true;
}
```

**الخطوات:**
1. جلب الدرجات الحالية
2. حساب المجموع
3. حساب النسبة المئوية
4. تحديد التقدير (Excellent, Very Good, ...)
5. حساب GPA (0-4)
6. تحديد الحرف (A, B, C, D, F)
7. حفظ النتائج

**4. calculateStudentCGPA():**
```cpp
static double calculateStudentCGPA(Database& db, const string& studentId) {
    auto stmt = db.prepareStatement(
        "SELECT COALESCE(SUM(g.gpa * c.credit_hours), 0) as total_points, "
        "COALESCE(SUM(c.credit_hours), 0) as total_credits "
        "FROM grades g "
        "JOIN registrations r ON g.registration_id = r.id "
        "JOIN courses c ON r.course_id = c.id "
        "WHERE r.student_id = ?"
    );
    stmt->setString(1, studentId);
    auto result = stmt->executeQuery();
    
    if (result->next()) {
        double totalPoints = result->getDouble("total_points");
        int totalCredits = result->getInt("total_credits");
        
        if (totalCredits > 0) {
            return totalPoints / totalCredits;  // المعدل التراكمي
        }
    }
    
    return 0.0;
}
```

**الصيغة:**
```
CGPA = (مجموع (GPA × عدد الساعات)) / (مجموع الساعات)
```

**مثال:**
- كورس 1: GPA = 4.0, ساعات = 3 → نقاط = 12
- كورس 2: GPA = 3.0, ساعات = 2 → نقاط = 6
- CGPA = (12 + 6) / (3 + 2) = 18 / 5 = 3.6

**لو سألوك:** ليه بنستخدم COALESCE؟
**الإجابة:** عشان لو مفيش نتائج (الطالب مسجلش أي كورس)، نرجع 0 بدل NULL

---

### 🔐 SessionManager Class

**الهدف:** إدارة الجلسات والصلاحيات (RBAC)

**UserSession Struct:**
```cpp
struct UserSession {
    string userId;              // رقم المستخدم
    string username;            // اسم المستخدم
    string fullName;            // الاسم الكامل
    string roleId;              // الدور (ROLE_STUDENT, ROLE_PROFESSOR, ...)
    string roleName;            // اسم الدور بالعربي
    string linkedId;            // رقم الطالب أو الأستاذ
    set<string> permissions;    // الصلاحيات
    bool isActive;              // مفعل؟
    
    bool hasPermission(const string& perm) const {
        return permissions.find(perm) != permissions.end();
    }
    
    bool isAdmin() const {
        return roleId == "ROLE_STUDENT_AFFAIRS" || roleId == "ROLE_SUPERADMIN";
    }
    
    bool isProfessor() const {
        return roleId == "ROLE_PROFESSOR";
    }
    
    bool isStudent() const {
        return roleId == "ROLE_STUDENT";
    }
};
```

**الدوال المهمة:**

**1. login():**
```cpp
static bool login(Database& db, const string& username, const string& password) {
    // التحقق من بيانات الدخول
    auto stmt = db.prepareStatement(
        "SELECT u.id, u.username, u.full_name, u.role_id, r.name_en as role_name "
        "FROM users u "
        "JOIN roles r ON u.role_id = r.id "
        "WHERE u.username = ? AND u.password_hash = ? AND u.is_active = TRUE"
    );
    stmt->setString(1, username);
    stmt->setString(2, password);
    auto result = stmt->executeQuery();
    
    if (result->next()) {
        // حفظ بيانات المستخدم في currentSession
        currentSession.userId = result->getString("id");
        currentSession.username = result->getString("username");
        currentSession.fullName = result->getString("full_name");
        currentSession.roleId = result->getString("role_id");
        currentSession.roleName = result->getString("role_name");
        currentSession.isActive = true;
        
        // جلب الصلاحيات
        currentSession.permissions = loadPermissions(db, currentSession.roleId, currentSession.userId);
        
        // جلب linkedId (رقم الطالب أو الأستاذ)
        if (currentSession.isProfessor()) {
            // جلب رقم الأستاذ
            auto profStmt = db.prepareStatement("SELECT id FROM professors WHERE user_id = ?");
            profStmt->setString(1, currentSession.userId);
            auto profResult = profStmt->executeQuery();
            if (profResult->next()) {
                currentSession.linkedId = profResult->getString("id");
            }
        } else if (currentSession.isStudent()) {
            // جلب رقم الطالب
            auto stuStmt = db.prepareStatement("SELECT id FROM students WHERE user_id = ?");
            stuStmt->setString(1, currentSession.userId);
            auto stuResult = stuStmt->executeQuery();
            if (stuResult->next()) {
                currentSession.linkedId = stuResult->getString("id");
            }
        }
        
        // تحديث last_login
        auto updateStmt = db.prepareStatement("UPDATE users SET last_login = CURRENT_TIMESTAMP WHERE id = ?");
        updateStmt->setString(1, currentSession.userId);
        updateStmt->executeUpdate();
        
        return true;
    }
    
    return false;
}
```

**2. loadPermissions():**
```cpp
static set<string> loadPermissions(Database& db, const string& roleId, const string& userId) {
    set<string> permissions;
    
    // جلب صلاحيات الدور
    auto stmt = db.prepareStatement(
        "SELECT p.name FROM permissions p "
        "JOIN role_permissions rp ON p.id = rp.permission_id "
        "WHERE rp.role_id = ?"
    );
    stmt->setString(1, roleId);
    auto result = stmt->executeQuery();
    
    while (result->next()) {
        permissions.insert(result->getString("name"));
    }
    
    // جلب صلاحيات المستخدم الخاصة (overrides)
    if (!userId.empty()) {
        auto userStmt = db.prepareStatement(
            "SELECT p.name, up.is_granted FROM user_permissions up "
            "JOIN permissions p ON p.id = up.permission_id "
            "WHERE up.user_id = ?"
        );
        userStmt->setString(1, userId);
        auto userResult = userStmt->executeQuery();
        
        while (userResult->next()) {
            string permName = userResult->getString("name");
            bool isGranted = userResult->getBoolean("is_granted");
            
            if (isGranted) {
                permissions.insert(permName);  // إضافة صلاحية
            } else {
                permissions.erase(permName);   // إزالة صلاحية
            }
        }
    }
    
    return permissions;
}
```

**الفكرة:**
1. كل دور (Role) له صلاحيات افتراضية
2. المستخدم ممكن يكون له صلاحيات إضافية أو ممنوعة
3. user_permissions تعدل على صلاحيات الدور

**3. hasPermission():**
```cpp
static bool hasPermission(const string& permission) {
    if (currentSession.isSuperAdmin()) {
        return true;  // Super Admin عنده كل الصلاحيات
    }
    return currentSession.hasPermission(permission);
}
```

**4. canAccessCourse():**
```cpp
static bool canAccessCourse(Database& db, const string& courseId, const string& semesterId) {
    if (currentSession.isAdmin()) {
        return true;  // الإدارة تقدر تشوف كل الكورسات
    }
    
    if (currentSession.isProfessor() && !currentSession.linkedId.empty()) {
        // التحقق من أن الأستاذ مسند له الكورس
        auto stmt = db.prepareStatement(
            "SELECT COUNT(*) as count FROM course_assignments "
            "WHERE professor_id = ? AND course_id = ? AND semester_id = ?"
        );
        stmt->setString(1, currentSession.linkedId);
        stmt->setString(2, courseId);
        stmt->setString(3, semesterId);
        auto result = stmt->executeQuery();
        
        if (result->next() && result->getInt("count") > 0) {
            return true;
        }
    }
    
    return false;
}
```

**لو سألوك:** ليه محتاجين canAccessCourse؟
**الإجابة:** عشان الأستاذ يشوف طلاب كورساته بس، مش كل الطلاب

---

### 🛠️ JsonHelper Class

**الهدف:** إنشاء وتحليل JSON بدون مكتبات خارجية

**الدوال:**

**1. escapeString():**
```cpp
static string escapeString(const string& str) {
    stringstream ss;
    for (char c : str) {
        switch (c) {
            case '"':  ss << "\\\""; break;  // " → \"
            case '\\': ss << "\\\\"; break;  // \ → \\
            case '\n': ss << "\\n"; break;   // newline
            case '\t': ss << "\\t"; break;   // tab
            default:
                if (c >= 0 && c < 32) {
                    ss << "\\u" << hex << setw(4) << (int)c;  // control chars
                } else {
                    ss << c;
                }
        }
    }
    return ss.str();
}
```

**2. makeString():**
```cpp
static string makeString(const string& value) {
    return "\"" + escapeString(value) + "\"";
}
```

**مثال:**
```cpp
makeString("Hello \"World\"")  →  "\"Hello \\\"World\\\"\""
```

**3. makeNumber():**
```cpp
static string makeNumber(int value) {
    return to_string(value);
}

static string makeNumber(double value) {
    stringstream ss;
    ss << fixed << setprecision(2) << value;
    return ss.str();
}
```

**مثال:**
```cpp
makeNumber(42)      →  "42"
makeNumber(3.14159) →  "3.14"
```

**4. makeArray():**
```cpp
static string makeArray(const vector<string>& items) {
    stringstream ss;
    ss << "[";
    for (size_t i = 0; i < items.size(); ++i) {
        if (i > 0) ss << ",";
        ss << items[i];
    }
    ss << "]";
    return ss.str();
}
```

**مثال:**
```cpp
vector<string> items = {"\"apple\"", "\"banana\""};
makeArray(items)  →  "[\"apple\",\"banana\"]"
```

**5. makeObject():**
```cpp
static string makeObject(const vector<pair<string, string>>& pairs) {
    stringstream ss;
    ss << "{";
    for (size_t i = 0; i < pairs.size(); ++i) {
        if (i > 0) ss << ",";
        ss << makeString(pairs[i].first) << ":" << pairs[i].second;
    }
    ss << "}";
    return ss.str();
}
```

**مثال:**
```cpp
vector<pair<string, string>> obj = {
    {"id", makeString("20241")},
    {"name", makeString("Ahmed")},
    {"age", makeNumber(20)}
};
makeObject(obj)  →  "{\"id\":\"20241\",\"name\":\"Ahmed\",\"age\":20}"
```

**6. parseSimpleValue():**
```cpp
static string parseSimpleValue(const string& json, const string& key) {
    // البحث عن "key":
    string searchKey = "\"" + key + "\"";
    size_t keyPos = json.find(searchKey);
    if (keyPos == string::npos) return "";
    
    // البحث عن :
    size_t colonPos = json.find(':', keyPos);
    if (colonPos == string::npos) return "";
    
    // تخطي المسافات
    size_t valueStart = colonPos + 1;
    while (valueStart < json.length() && isspace(json[valueStart])) {
        valueStart++;
    }
    
    // استخراج القيمة
    if (json[valueStart] == '"') {
        // قيمة نصية
        size_t valueEnd = json.find('"', valueStart + 1);
        return json.substr(valueStart + 1, valueEnd - valueStart - 1);
    } else {
        // رقم أو boolean
        size_t valueEnd = json.find_first_of(",}]", valueStart);
        return json.substr(valueStart, valueEnd - valueStart);
    }
}
```

**مثال:**
```cpp
string json = "{\"id\":\"20241\",\"name\":\"Ahmed\"}";
parseSimpleValue(json, "id")   →  "20241"
parseSimpleValue(json, "name") →  "Ahmed"
```

**لو سألوك:** ليه مش بنستخدم مكتبة JSON جاهزة زي nlohmann/json؟
**الإجابة:** عشان المشروع بسيط ومش محتاج features معقدة، والكود اليدوي أخف وأسرع

---

## ❓ أسئلة متوقعة وإجاباتها

### عن المشروع:

**س1: إيه هو الـ RBAC وإزاي بيشتغل في المشروع؟**
ج: RBAC = Role-Based Access Control
- كل مستخدم عنده دور (Role): Student, Professor, Student Affairs, Super Admin
- كل دور عنده صلاحيات (Permissions): student.create, grade.enter, إلخ
- قبل أي عملية، بنتحقق من الصلاحية: `SessionManager::hasPermission()`
- الصلاحيات محفوظة في جداول: roles, permissions, role_permissions

**س2: إزاي بيتم الاتصال بين C++ والـ frontend؟**
ج: عن طريق WebView:
1. الـ C++ بيستخدم `w.bind()` عشان يربط functions
2. الـ JavaScript بينادي على الـ functions دي: `window.cpp_login({...})`
3. الـ C++ بيرجع JSON response
4. الـ JavaScript بيستقبل الـ response ويحدث الواجهة

**س3: ليه بنستخدم transactions في قاعدة البيانات؟**
ج: عشان نضمن Data Integrity:
- لو عملية فشلت في النص، كل العمليات السابقة بترجع (ROLLBACK)
- مثال: لما بنضيف طالب، بننشئ user و student، لو واحدة فشلت، الاتنين يلغوا
- `START TRANSACTION` → `COMMIT` (نجاح) أو `ROLLBACK` (فشل)

**س4: إيه الفرق بين executeQuery و executeUpdate؟**
ج:
- `executeQuery()`: لـ SELECT (قراءة) → يرجع ResultSet
- `executeUpdate()`: لـ INSERT/UPDATE/DELETE (كتابة) → يرجع عدد الصفوف المتأثرة

**س5: ليه بنستخدم Prepared Statements؟**
ج:
1. **أمان**: حماية من SQL Injection
2. **أداء**: MySQL بتعمل optimize للـ query
3. **سهولة**: مفيش concatenation معقد

**س6: إزاي بيتم حساب المعدل التراكمي CGPA؟**
ج:
```
CGPA = مجموع (GPA × عدد الساعات) / مجموع الساعات
```
مثال:
- كورس 1: GPA = 4.0, ساعات = 3 → نقاط = 12
- كورس 2: GPA = 3.0, ساعات = 2 → نقاط = 6
- CGPA = (12 + 6) / (3 + 2) = 3.6

**س7: إيه الفرق بين static و instance methods؟**
ج:
- **Static**: زي `Student::exists(db, id)` → بنناديها من الـ class مباشرة
- **Instance**: زي `student.save(db)` → محتاجين object عشان نناديها

**س8: ليه بنستخدم unique_ptr؟**
ج: Smart Pointer بيحرر الذاكرة أوتوماتيكياً:
- لما الـ unique_ptr يخرج من الـ scope، بيحذف الـ object
- مفيش حاجة اسمها memory leak
- أفضل من استخدام `new` و `delete` اليدوي

**س9: إزاي بيتم تخزين الباسوورد؟**
ج: في المشروع ده، الباسوورد بيتخزن plain text (غير آمن!)
في production، لازم:
1. استخدام hashing (SHA256, bcrypt)
2. إضافة salt
3. عدم تخزين الباسوورد الأصلي أبداً

**س10: ليه بنستخدم LEFT JOIN؟**
ج: عشان نجيب كل الصفوف من الجدول الأيسر حتى لو مفيش match في الجدول الأيمن:
```sql
SELECT * FROM students s 
LEFT JOIN users u ON s.user_id = u.id
```
لو الطالب مالوش user، يظهر بـ NULL في بيانات الـ user

---

## 🎯 نصائح للمذاكرة والامتحان

### 1. افهم الـ Flow:
- تتبع مسار تسجيل الدخول: من الـ frontend → cpp_login → SessionManager → Database
- تتبع مسار إضافة طالب: Frontend → cpp_addStudent → Student::save() → Database

### 2. اعرف الـ Classes الأساسية:
- **Database**: الاتصال بقاعدة البيانات
- **Student**: بيانات وعمليات الطلاب
- **Grade**: حساب الدرجات
- **SessionManager**: الصلاحيات والجلسات
- **JsonHelper**: التعامل مع JSON

### 3. اعرف الفرق بين:
- Static vs Instance methods
- executeQuery vs executeUpdate
- INNER JOIN vs LEFT JOIN
- save vs update vs remove

### 4. اعرف ليه:
- بنستخدم Transactions
- بنستخدم Prepared Statements
- بنستخدم RBAC
- بنستخدم WebView

### 5. كلمات مفتاحية:
- **RBAC**: Role-Based Access Control
- **Transaction**: مجموعة عمليات إما تنجح كلها أو تفشل كلها
- **Prepared Statement**: استعلام جاهز بمعاملات (حماية من SQL Injection)
- **Session**: بيانات المستخدم الحالي
- **Permission**: صلاحية للقيام بعملية

---

## 📋 خلاصة الملفات

| الملف | الغرض |
|-------|-------|
| `main.cpp` | نقطة دخول البرنامج، ربط C++ بالواجهة |
| `Database.h` | الاتصال بـ MySQL وتنفيذ الاستعلامات |
| `Student.h/cpp` | إدارة بيانات الطلاب (CRUD) |
| `Professor.h/cpp` | إدارة بيانات الأساتذة |
| `Course.h/cpp` | إدارة المقررات |
| `Grade.h/cpp` | حساب وإدارة الدرجات |
| `SessionManager.h/cpp` | الصلاحيات وإدارة الجلسات |
| `JsonHelper.h/cpp` | إنشاء وتحليل JSON |
| `Attendance.h/cpp` | تسجيل الحضور |
| `Registration.h/cpp` | تسجيل الطلاب في المقررات |

---

## 🔥 نهاية الشرح

**بالتوفيق في الامتحان! 🎓**

**لو عندك أي سؤال، ارجع للملف ده، هتلاقي الإجابة.**

---

## 📝 ملاحظات إضافية

### التحسينات الممكنة:
1. **استخدام hashing للباسوورد** (bcrypt, SHA256)
2. **استخدام JSON library** (nlohmann/json)
3. **إضافة input validation أقوى**
4. **إضافة unit tests**
5. **استخدام connection pooling** للـ database
6. **إضافة logging أفضل**
7. **فصل business logic عن presentation**

### المشاكل الحالية:
1. **الباسوورد plain text** ⚠️ خطر أمني
2. **لا توجد rate limiting** (حماية من brute force)
3. **لا توجد session timeout**
4. **لا يوجد HTTPS** (البيانات غير مشفرة)

---

**ملحوظة مهمة:** 
المشروع ده تعليمي، لو هتستخدمه في production لازم تعمل التحسينات الأمنية المذكورة! 🔒
