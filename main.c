#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    int id;
    char name[50];
    int age;
    float gpa;
} Student;

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

int load_from_file(Student *db, const char *filename) {
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

    while (fgets(line, sizeof(line), storage) != NULL && count < 100) {
        if (line[0] == '\n' || line[0] == '\0') continue;

        int id, age;
        char name[50];
        float gpa;

        int fields = sscanf(line, "%d,%49[^,],%d,%f", &id, name, &age, &gpa);
        if (fields != 4) {
            fprintf(stderr, "Skipping malformed line: %s", line);
            continue;
        }

        db[count].id = id;
        strcpy(db[count].name, name);
        db[count].age = age;
        db[count].gpa = gpa;
        count++;
    }

    fclose(storage);
    return count;
}

// ---------- Database operations ----------

void insert(Student *db, int *count, Student s) {
    if (*count >= 100) {
        printf("Database is full\n");
        return;
    }
    db[*count] = s;
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

int update_gpa(Student *db, int count, int id, float gpa) {
    int idx = find_by_id(db, count, id);
    if (idx == -1) return 0;
    db[idx].gpa = gpa;
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

// ---------- Main ----------

int main(void) {
    Student db[100];
    int count = load_from_file(db, "db.csv");
    int running = 1;
    int choice;

    while (running) {
        printf("\n1. Insert \n2. List \n3. Search \n4. Update \n5. Delete \n6. Exit\nChoice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            clear_input_buffer();
            continue;
        }

        switch (choice) {
            case 1: {
                if (count >= 100) {
                    printf("Database is full\n");
                    break;
                }
                Student new_student;

                printf("Enter student fields:\n");
                printf("int id, char name[50] (no commas), int age, float gpa\n");
                scanf("%d", &new_student.id);
                scanf("%49s", new_student.name);
                scanf("%d", &new_student.age);
                scanf("%f", &new_student.gpa);

                insert(db, &count, new_student);
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
                scanf("%d", &id);
                if (id <= 0) {
                    printf("Invalid ID!\n");
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
                float gpa;
                printf("Enter ID and new GPA for a student: ");
                scanf("%d", &id);
                scanf("%f", &gpa);
                if (update_gpa(db, count, id, gpa)) {
                    printf("Updated successfully!\n");
                } else {
                    printf("Not found.\n");
                }
                break;
            }
            case 5: {
                int id;
                printf("Enter ID to delete: ");
                scanf("%d", &id);
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
                break;
            }
            default:
                printf("Invalid choice.\n");
                break;
        }
    }

    return 0;
}