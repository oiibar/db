#include <stdio.h>
#include <string.h>

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
        printf("GPA: %.2f\n", db[i].gpa);
    }
    printf("Done! \n");
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


int main(void) {
    Student db[100];
    int count = 0;
    int choice;

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
                print_all(db, count);
                break;
            }
            default:
                break;
        }
    }

    return 0;
}