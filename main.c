#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    int id;
    char name[50];
    int age;
    float gpa;
} Student;

// ---------- Prototypes ----------
int save_to_file(Student *db, int count, const char *filename);
int load_from_file(Student **db, int *max_size, const char *filename);
int resize(Student **db, int *max_size);
int compare_by_gpa_asc(const void* a, const void* b);
int compare_by_age(const void* a, const void* b);
int compare_by_name(const void* a, const void* b);
int read_line(char *buf, int bufsize);
void clear_input_buffer(void);
void insert(Student **db, int *count, int *max_size, Student s);
int print_all(Student *db, int count);
int find_by_id(Student *db, int count, int id);
int update(Student *db, int count, int id);
int delete_by_id(Student *db, int *count, int id);

// ---------- File I/O (CSV) ----------

int save_to_file(Student *db, int count, const char *filename) {
    FILE *storage = fopen(filename, "w");
    if (storage == NULL) {
        perror("Error opening file for writing");
        return 0;
    }

    fprintf(storage, "id,name,age,gpa\n");

    for (int i = 0; i < count; i++) {
        fprintf(storage, "%d,%s,%d,%.2f\n",
                db[i].id, db[i].name, db[i].age, db[i].gpa);
    }

    fclose(storage);
    return 1;
}

int load_from_file(Student **db, int *max_size, const char *filename) {
    FILE *storage = fopen(filename, "r");
    if (storage == NULL) {
        return 0;
    }

    char line[256];
    int count = 0;

    if (fgets(line, sizeof(line), storage) == NULL) { 
        fclose(storage);
        return 0;
    }

    while (fgets(line, sizeof(line), storage) != NULL) {
        if (line[0] == '\n' || line[0] == '\0') continue;

        if (count >= *max_size) {
            if (resize(db, max_size) != 0) {
                fprintf(stderr, "Could not grow database while loading file; remaining records skipped.\n");
                break;
            }
        }

        int id, age;
        char name[50];
        float gpa;

        int fields = sscanf(line, "%d,%49[^,],%d,%f", &id, name, &age, &gpa);
        if (fields != 4) {
            fprintf(stderr, "Skipping malformed line: %s", line);
            continue;
        }

        (*db)[count].id = id;
        strcpy((*db)[count].name, name);
        (*db)[count].age = age;
        (*db)[count].gpa = gpa;
        count++;
    }

    fclose(storage);
    return count;
}

// ---------- Database operations ----------

int resize(Student **db, int *max_size) {
    int new_size = (*max_size) * 2;
    Student *temp = realloc(*db, (size_t)new_size * sizeof(Student));
    if (temp == NULL) {
        fprintf(stderr, "Memory reallocation failed! Original array is still intact.\n");
        return 1; 
    }
    *db = temp;
    *max_size = new_size;
    return 0;
}

int read_line(char *buf, int bufsize) {
    if (fgets(buf, bufsize, stdin) == NULL) {
        buf[0] = '\0';
        return 0;
    }

    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
        len--;
    }

    return len > 0 ? 1 : 0;
}

void insert(Student **db, int *count, int *max_size, Student s) {
    if (*count >= *max_size) {
        if (resize(db, max_size) != 0) {
            printf("Failed to resize. Insert aborted.\n");
            return;
        }
        printf("Database resized to %d.\n", *max_size);
    }
    (*db)[*count] = s;
    (*count)++;
}

int print_all(Student *db, int count) {
    if (count == 0) {
        printf("No students in database.\n");
        return 0;
    }
    for (int i = 0; i < count; i++) {
        printf("ID: %d\n", db[i].id);
        printf("Name: %s\n", db[i].name);
        printf("Age: %d\n", db[i].age);
        printf("GPA: %.2f\n\n", db[i].gpa);
    }
    return 1;
}

int find_by_id(Student *db, int count, int id) {
    for (int i = 0; i < count; i++) {
        if (db[i].id == id) {
            return i;
        }
    }
    return -1;
}

int update(Student *db, int count, int id) {
    int idx = find_by_id(db, count, id);
    if (idx == -1) return 0;

    printf("Current record: Name=%s, Age=%d, GPA=%.2f\n",
           db[idx].name, db[idx].age, db[idx].gpa);
    
    char line[256];

    // --- Name ---
    printf("Enter new name (or press Enter to keep \"%s\"): ", db[idx].name);
    if (read_line(line, sizeof(line))) {
        strncpy(db[idx].name, line, sizeof(db[idx].name) - 1);
        db[idx].name[sizeof(db[idx].name) - 1] = '\0';
    }

    // --- Age ---
    printf("Enter new age (or press Enter to keep %d): ", db[idx].age);
    if (read_line(line, sizeof(line))) {
        char *endptr;
        long val = strtol(line, &endptr, 10);
        if (endptr == line || *endptr != '\0') {
            printf("  Invalid age input — keeping current value.\n");
        } else {
            db[idx].age = (int)val;
        }
    }

    // --- GPA ---
    printf("Enter new GPA (or press Enter to keep %.2f): ", db[idx].gpa);
    if (read_line(line, sizeof(line))) {
        char *endptr;
        double val = strtod(line, &endptr);
        if (endptr == line || *endptr != '\0') {
            printf("  Invalid GPA input — keeping current value.\n");
        } else {
            db[idx].gpa = (float)val;
        }
    }

    return 1;
}

int delete_by_id(Student *db, int *count, int id) {
    int idx = find_by_id(db, *count, id);
    if (idx == -1) {
        return 0;
    }
    for (int i = idx; i < *count - 1; i++) {
        db[i] = db[i + 1];
    }
    (*count)--;
    return 1;
}

void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int compare_by_gpa_asc(const void* a, const void* b) {
    const Student *s1 = (const Student *)a;
    const Student *s2 = (const Student *)b;

    if(s1->gpa > s2->gpa) return -1;
    if(s1->gpa < s2->gpa) return 1;
    return 0;
}

int compare_by_name(const void *a, const void *b) {
    const Student *s1 = (const Student *)a;
    const Student *s2 = (const Student *)b;
    
    return strcmp(s1->name, s2->name);
}

int compare_by_age(const void* a, const void* b) {
    const Student *s1 = (const Student *)a;
    const Student *s2 = (const Student *)b;

    if(s1->age < s2->age) return -1;
    if(s1->age > s2->age) return 1;
    return 0;
}

// ---------- Main ----------

int main(void) {
    int max_size = 100;
    Student *db = malloc((size_t)max_size * sizeof(Student));
    if (db == NULL) {
        fprintf(stderr, "Initial allocation failed.\n");
        return 1;
    }

    int count = load_from_file(&db, &max_size, "db.csv");
    int running = 1;
    int choice;

    while (running) {
        printf("\n1. Insert \n2. List \n3. Search \n4. Update \n5. Delete \n6. Exit\n7. Sort\nChoice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            clear_input_buffer();
            continue;
        }

        switch (choice) {
            case 1: {
                Student new_student;

                printf("Enter student fields:\n");
                printf("int id, name (spaces ok, no commas), int age, float gpa\n");

                if (scanf("%d", &new_student.id) != 1 ||
                    scanf(" %49[^\n]", new_student.name) != 1 ||
                    scanf("%d", &new_student.age) != 1 ||
                    scanf("%f", &new_student.gpa) != 1) {
                    printf("Invalid input — insert cancelled.\n");
                    clear_input_buffer();
                    break;
                }

                insert(&db, &count, &max_size, new_student);
                printf("Inserted successfully!\n");
                break;
            }
            case 2: {
                print_all(db, count);
                break;
            }
            case 3: {
                int id;
                printf("Enter student ID to search by ID: ");
                if (scanf("%d", &id) != 1 || id <= 0) {
                    printf("Invalid ID!\n");
                    clear_input_buffer();
                    break;
                }
                int idx = find_by_id(db, count, id);
                if (idx == -1) {
                    printf("Not found.\n");
                } else {
                    printf("Found at idx: %d\n", idx);
                }
                break;
            }
            case 4: {
                int id;
                printf("Enter ID to update: ");
                if (scanf("%d", &id) != 1) {
                    printf("Invalid input.\n");
                    clear_input_buffer();
                    break;
                }
                clear_input_buffer(); 

                if (update(db, count, id)) {
                    printf("Updated successfully!\n");
                } else {
                    printf("Not found.\n");
                }
                break;
            }
            case 5: {
                int id;
                printf("Enter ID to delete: ");
                if (scanf("%d", &id) != 1) {
                    printf("Invalid input.\n");
                    clear_input_buffer();
                    break;
                }
                if (delete_by_id(db, &count, id)) {
                    printf("Deleted successfully!\n");
                } else {
                    printf("Not found.\n");
                }
                break;
            }
            case 6: {
                printf("Saving and Exiting...\n");
                save_to_file(db, count, "db.csv");
                running = 0;
                free(db);
                break;
            }
            case 7: {
                printf("Sort by: 1=Name 2=Age 3=GPA\n");
                int sort_choice;
                if (scanf("%d", &sort_choice) != 1) {
                    printf("Invalid input.\n");
                    clear_input_buffer();
                    break;
                }
                switch (sort_choice) {
                    case 1: qsort(db, count, sizeof(Student), compare_by_name); break;
                    case 2: qsort(db, count, sizeof(Student), compare_by_age);  break;
                    case 3: qsort(db, count, sizeof(Student), compare_by_gpa_asc);  break;
                    default: printf("Invalid choice.\n"); break;
                }
                printf("Sorted.\n");
                break;
            }
            default:
                printf("Invalid choice.\n");
                break;
        }
    }

    return 0;
}