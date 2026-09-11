#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    int id;
    char name[50];
    int age;
    float gpa;
} Student;

void insert(Student *db, int *count, Student s) {
    if(*count >= 100) {
        printf("Database if full!\n");
        return;
    }
    db[*count] = s;
    (*count)++;
}

void print_all(Student *db, int count) {
    printf("All records: \n");
    for(int i = 0; i < count; i++) {
        printf("ID: %d\n", db[i].id);
        printf("Name: %s\n", db[i].name);
        printf("Age: %d\n", db[i].age);
        printf("GPA: %.2f\n\n", db[i].gpa);
    }
}

int find_by_id(Student *db, int count, int id) {
    for(int i = 0; i < count; i++) {
        if(db[i].id == id) {
            return i;
        }
    }
    return -1;
}

void update_gpa(Student *db, int count, int id, float gpa) {
    int idx = find_by_id(db, count, id);
    if(idx == -1) {
        printf("Not found.\n");
        return;
    }
    db[idx].gpa = gpa;
}

void delete_by_id(Student *db, int *count, int id) {
    int idx = find_by_id(db, *count, id);
    if (idx == -1) {
        printf("Not found.\n");
        return;
    }
    for(int i = idx; i < *count - 1; i++) {
        db[i] = db[i + 1];
    }
    (*count)--;
}

// void write_to_db(Student *db, int *count) {
//     FILE *storage = fopen("db.bin", "wb");
//     if(storage == NULL) {
//         perror("Error opening db");
//         return;
//     }
//     int size_of_entry = sizeof(Student);
//     size_t written = fwrite(db, size_of_entry, 1, storage);

//     if(written != *count) {
//         perror("Error writing data to db");
//         fclose(db);
//         return;
//     }

//     fclose(storage);
//     printf("Successfully saved 1 record to db.bin\n");
// }

// void read_from_db(Student *db, int count) {
//     FILE *storage = fopen("employees.bin", "rb");
//     if (storage == NULL) {
//         perror("Error opening storage");
//         return;
//     }

//     Student result[1];

//     size_t items_read = fread(result, sizeof(Student), count, storage);

//     printf("Successfully read %zu items.\n", items_read);

//     fclose(storage);

//     if (items_read > 0) {
//         printf("First Student ID: %d\n", result[0].id);
//     }
// }

int main(void) {
    Student db[100];
    int count = 0;
    int choice;

    Student init_student;
    init_student.id = 0;
    strcpy(init_student.name, "Admin");
    init_student.age = 99;
    init_student.gpa = 4.0;

    insert(db, &count, init_student);

    while(1) {
        printf("\n1. Insert \n2. List \n3. Search \n4. Update \n5. Delete \n6. Exit\nChoice: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1: {
                int id;
                int age;
                char name[50];
                float gpa;
                Student new_student;
 
                printf("Enter student fields:\n");
                printf("int id, char name[50], int age, float gpa\n");
                scanf("%d", &id);
                scanf("%s", name);
                scanf("%d", &age);
                scanf("%f", &gpa);

                new_student.id = id;
                strcpy(new_student.name, name);
                new_student.age = age;
                new_student.gpa = gpa;

                insert(db, &count, new_student);
                break;
            }
            case 2: {
                print_all(db, count);
                break;
            }
            case 3: {
                int id;
                printf("Enter student ID to search by ID:\n");
                scanf("%d\n", &id);
                if(!id) {
                    perror("Invalid ID!");
                    break;
                }
                find_by_id(db, count, id);
                printf("Found at idx: %d", id);
            }
            case 4: {
                int id;
                float gpa;
                printf("Enter ID and new GPA for a student: ");
                scanf("%d\n", &id);
                scanf("%f\n", &gpa);
                update_gpa(db, count, id, gpa);
                printf("Updated successfully!");
            }
            case 5: {
                int id;
                printf("Enter ID to delete: ");
                scanf("%d\n", &id);
                delete_by_id(db, &count, id);
                printf("Deleted successfully!");
            }
            case 6: {
                printf("Exiting...\n");
                exit(0);
            }
            default:
                break;
        }
    }

    return 0;
}