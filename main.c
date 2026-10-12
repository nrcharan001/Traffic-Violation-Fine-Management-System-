#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

typedef struct locs_tag locations;
typedef struct vehicle_tag vehicle;
typedef struct violations_tag violations;
typedef struct PaymentNode Payments;

typedef enum{
    illegal_parking,
    over_speeding,
    red_light_jump,
    helmet_seatbelt_infraction,
    others
}type_of_violation;

typedef struct vehicle_tag{
    char reg_no[15];        //key =1
    char owner_name[30];
    long long int phone_no;
    char *address;
    int total_fines_accured;    //key=2
    int total_fines_paid;       //key=3
    int pending_amount;         //key=4
    int no_of_violations;       //key=5
    int no_of_unpaid_violations; //key=6
    struct vehicle_tag *next;
}vehicle;

typedef struct locs_tag{
    int location_id;
    char area[60];
    int no_of_violations;   //key 1
    int revenue_generated;   //key 2
    struct locs_tag *next;
}locations;

typedef struct violations_tag{
    int violation_id;
    vehicle *vh;
    locations *loc;
    type_of_violation type_vio;
    char datetime[20];
    bool ispaid;
    float fine_amount;
    struct violations_tag *next;
}violations;



typedef struct PaymentNode {
    int transaction_id;
    violations *violation;
    float amount_paid;
    char payment_mode[20];
    char date[11];
    char *reg_no;
    struct PaymentNode *next;
} Payments;

locations *location_head=NULL;
vehicle *vehicle_head=NULL;
violations *violations_head=NULL;
Payments *payments_stack_top=NULL;

vehicle * helper_find_vehicle(const char * reg_n0){
    vehicle *temp=vehicle_head;
    while(temp!=NULL){
        if(strcmp(temp->reg_no,reg_n0)==0){
            return temp;
        }
        temp=temp->next;
    }
    return NULL;
}

locations * helper_find_location(int loc_id){
    locations *temp=location_head;
    while(temp!=NULL){
        if(temp->location_id==loc_id){
            return temp;
        }
        temp=temp->next;
    }
    return NULL;
}

violations * helper_find_violation(int violation_id){
    violations *temp=violations_head;
    while(temp!=NULL){
        if(temp->violation_id==violation_id){
            return temp;
        }
        temp=temp->next;
    }
    return NULL;
}

typedef struct {
    char type_name[50];
}type_name_mapping;

type_name_mapping type_names[] = {
    {"Illegal Parking"},
    {"Over Speeding"},
    {"Red Light Jump"},
    {"Helmet/Seatbelt Infraction"},
    {"Others"}
};

void helper_print_locations(){
    locations *ptr=location_head;
    if(ptr==NULL){
        printf("No locations found.\n");
        return;
    }
    while(ptr!=NULL){
        printf("Location ID: %d, Area: %s, No of Violations: %d, Revenue Generated: %d\n",
               ptr->location_id, ptr->area, ptr->no_of_violations, ptr->revenue_generated);
        ptr=ptr->next;
    }
}
void main_print_locations(){
    locations *ptr=location_head;
    if(ptr==NULL){
        printf("No locations found.\n");
        return;
    }
    printf("Available Locations:\n");
    while(ptr!=NULL){
        printf("Location ID: %d, Area: %s\n", ptr->location_id, ptr->area);
        ptr=ptr->next;
    }
}

//1


void insert_vehicle_sorted(vehicle *new_vehicle){
    if(vehicle_head==NULL || strcmp(new_vehicle->reg_no,vehicle_head->reg_no)<0){
        new_vehicle->next=vehicle_head;
        vehicle_head=new_vehicle;
        return;
    }
    vehicle *current=vehicle_head;
    while(current->next!=NULL && strcmp(current->next->reg_no,new_vehicle->reg_no)<0){
        current=current->next;
    }
    new_vehicle->next=current->next;
    current->next=new_vehicle;
}


//3

void process_payment(int violation_id, float amount_paid, const char *payment_mode, const char *date) {
    violations *violation = helper_find_violation(violation_id);
    if (violation == NULL) {
        printf("Violation ID %d not found.\n", violation_id);
        return;
    }

    vehicle *veh = violation->vh;
    if (veh == NULL || violation->ispaid) {
        printf("Vehicle associated with Violation ID %d not found.\n", violation_id);
        return;
    }
    // Update the vehicle's payment details
    amount_paid = (amount_paid > violation->fine_amount) ? violation->fine_amount : amount_paid;
    veh->no_of_unpaid_violations -= 1;
    veh->total_fines_paid += amount_paid;
    veh->pending_amount -= amount_paid;
    locations *loc = violation->loc;
   
    // Create a new payment record
    violation->ispaid = true;

    Payments *new_payment = (Payments *)malloc(sizeof(Payments));
    new_payment->transaction_id = rand(); 

    //Generate a random transaction ID

    new_payment->violation = violation;
    new_payment->amount_paid = amount_paid;
    strncpy(new_payment->payment_mode, payment_mode, sizeof(new_payment->payment_mode) - 1);
    strncpy(new_payment->date, date, sizeof(new_payment->date) - 1);
    new_payment->next = payments_stack_top;
    new_payment->reg_no = veh->reg_no;
    payments_stack_top = new_payment;

    //update revenue generated for the location

    loc->revenue_generated += amount_paid;
    printf("Payment of %.2f for Violation ID %d processed successfully.\n", amount_paid, violation_id);
}

//12
void display_violations_of_vehicle(const char *reg_no){
    vehicle *veh=helper_find_vehicle(reg_no);
    violations *ptr=violations_head;
    int no_pending=0;
    if(veh==NULL){
        printf("Vehicle with registration number %s not found.\n", reg_no);
        return;
    }
    while(ptr!=NULL){
        if(ptr->vh==veh && !ptr->ispaid){
            printf("Violation ID: %d, Type: %s, Amount: %.2f\n", ptr->violation_id, type_names[ptr->type_vio].type_name, ptr->fine_amount);
            no_pending++;
        }
        ptr=ptr->next;
    }
    if(no_pending==0){
        printf("No pending violations found for vehicle with registration number %s.\n", reg_no);
    }

}

//11

void display_vehicle_above_threshold(int val){
    vehicle *ptr=vehicle_head;
    if(ptr==NULL){
        printf("No vehicles found.\n");
        return;

    }
    while(ptr!=NULL){
        if(ptr->no_of_violations >=val){
            printf(" Reg No: %s \n Owner: %s \n Phone: %lld \n Address: %s \n Total Fines Accrued: %d \n Total Fines Paid: %d \n Pending Amount: %d \n No of Violations: %d\n \n",
                   ptr->reg_no, ptr->owner_name, ptr->phone_no, ptr->address,
                   ptr->total_fines_accured, ptr->total_fines_paid,
                   ptr->pending_amount, ptr->no_of_violations);

        }
        ptr=ptr->next;
    }
}

vehicle * helper_divide(vehicle * head){
    if(head!=NULL && head->next!=NULL){
        vehicle *ptr1=head;
        vehicle *ptr2=head->next;
        while(ptr2!=NULL && ptr2->next!=NULL){
            ptr1=ptr1->next;
            ptr2=ptr2->next->next;
        }
        vehicle *temp=ptr1->next;
        ptr1->next=NULL;
        return temp;
    }
    return NULL;
}
int helper_compare(vehicle *v1,vehicle *v2,int key){
    switch(key){
        case 1:
            return strcmp(v1->reg_no,v2->reg_no);
        case 2:
            return v1->total_fines_accured-v2->total_fines_accured;
        case 3:
            return v1->total_fines_paid-v2->total_fines_paid;
        case 4:
            return v1->pending_amount-v2->pending_amount;
        case 5:
            return v1->no_of_violations-v2->no_of_violations;
        case 6:
            return(v1->no_of_unpaid_violations-v2->no_of_unpaid_violations);
    }
}

vehicle * helper_merge_sorted_lists(vehicle *v1,vehicle *v2,int key){
    vehicle *vptr1=v1;
    vehicle *vptr2=v2;
    vehicle *result=(vehicle *)malloc(sizeof(vehicle));
    vehicle *tail=result;
    while(vptr1!=NULL && vptr2!=NULL){
        if(helper_compare(vptr1,vptr2,key)<=0){
            tail->next=vptr1;
            vptr1=vptr1->next;
        }
        else{
            tail->next=vptr2;
            vptr2=vptr2->next;
        }
        tail = tail->next; 
    }
    if(vptr1!=NULL){
        tail->next=vptr1;
    }
    if(vptr2!=NULL){
        tail->next=vptr2;
    }
    return result->next;
}
void helper_sort_vehicles(vehicle **head,int key){
    if(*head==NULL || (*head)->next==NULL){
        return;
    }
    vehicle *second_half=helper_divide(*head);
    helper_sort_vehicles(head,key);
    helper_sort_vehicles(&second_half,key);
    *head=helper_merge_sorted_lists(*head,second_half,key);
}

void main_sort_vehicles(int key){
    helper_sort_vehicles(&vehicle_head,key);
}

void main_print_vehicles(){
    vehicle *ptr=vehicle_head;
    if(ptr==NULL){
        printf("No vehicles found.\n");
        return;
    }
    while(ptr!=NULL){
        printf(" Reg No: %s \n Owner: %s \n Phone: %lld \n Address: %s \n Total Fines Accrued: %d \n Total Fines Paid: %d \n Pending Amount: %d \n No of Violations: %d\n \n",
               ptr->reg_no, ptr->owner_name, ptr->phone_no, ptr->address,
               ptr->total_fines_accured, ptr->total_fines_paid,
               ptr->pending_amount, ptr->no_of_violations);
        ptr=ptr->next;
    }
}

void main_print_violations(){
    violations *ptr=violations_head;
    if(ptr==NULL){
        printf("No violations found.\n");
        return;
    }
    while(ptr!=NULL){
        printf("Violation ID: %d, Vehicle Reg No: %s, Location ID: %d, Type: %s, DateTime: %s, Is Paid: %d, Fine Amount: %.2f\n",
               ptr->violation_id, ptr->vh->reg_no, ptr->loc->location_id,
               type_names[ptr->type_vio].type_name, ptr->datetime, ptr->ispaid, ptr->fine_amount);
        ptr=ptr->next;
    }
}

//4

void main_find_vehicles_and_violations(char *owner_name){
    vehicle *v_ptr=vehicle_head;
    violations *vio_ptr=violations_head;
    int found=0;

    while(v_ptr!=NULL){
        if(strcmp(v_ptr->owner_name, owner_name)==0){
            printf("Vehicle found:\n");
            printf("Reg No: %s \n Owner: %s \n Phone: %lld \n Address: %s \n Total Fines Accrued: %d \n Total Fines Paid: %d \n Pending Amount: %d \n No of Violations: %d\n \n",
                   v_ptr->reg_no, v_ptr->owner_name, v_ptr->phone_no, v_ptr->address,
                   v_ptr->total_fines_accured, v_ptr->total_fines_paid,
                   v_ptr->pending_amount, v_ptr->no_of_violations);
            found=1;
        }
        v_ptr=v_ptr->next;
    }

    if(!found){
        printf("No vehicles found for owner: %s\n", owner_name);
    }

    while(vio_ptr!=NULL){
        if(strcmp(vio_ptr->vh->owner_name, owner_name)==0){
            printf("Violation found:\n");
            printf("Violation ID: %d, Vehicle Reg No: %s, Location ID: %d, Type: %s, DateTime: %s, Is Paid: %d, Fine Amount: %.2f\n",
                   vio_ptr->violation_id, vio_ptr->vh->reg_no, vio_ptr->loc->location_id,
                   type_names[vio_ptr->type_vio].type_name, vio_ptr->datetime, vio_ptr->ispaid, vio_ptr->fine_amount);
        }
        vio_ptr=vio_ptr->next;
    }
}
void reverse_vehicles(){
    vehicle *prev=NULL;
    vehicle *current=vehicle_head;
    vehicle *next=NULL;
    while(current!=NULL){
        next=current->next;
        current->next=prev;
        prev=current;
        current=next;
    }
    vehicle_head=prev;
}

//13


void main_display_vehicles_descending_by_pending_amount(){
    main_sort_vehicles(4);
    reverse_vehicles();
    main_print_vehicles();
}

//helper 1
void sort_vehicles_by_reg_no(){
    main_sort_vehicles(1);
}

void sort_vehicles_by_total_fines_accured(){
    main_sort_vehicles(2);
}

void sort_vehicles_by_total_fines_paid(){
    main_sort_vehicles(3);
}

void sort_vehicles_by_no_of_violations(){
    main_sort_vehicles(5);
}

//6


void main_display_vehicles_descending_by_unpaid_violations(){
    main_sort_vehicles(6);
    reverse_vehicles();
    main_print_vehicles();
}

locations * helper_divide_locations(locations * head){
    if(head!=NULL && head->next!=NULL){
        locations *ptr1=head;
        locations *ptr2=head->next;
        while(ptr2!=NULL && ptr2->next!=NULL){
            ptr1=ptr1->next;
            ptr2=ptr2->next->next;
        }
        
        locations *temp = ptr1->next; 
        ptr1->next = NULL;
        return temp;
    }
    return NULL;
}

int compare_locations_by_key_descending(locations *l1,locations *l2,int key){
    switch(key){
        case 1:
            return l1->no_of_violations-l2->no_of_violations;
        case 2:
            return l1->revenue_generated-l2->revenue_generated;
    }
}

locations * helper_merge_locations_descending(locations *l1,locations *l2,int key){
    locations *lptr1=l1;
    locations *lptr2=l2;
    locations *result=(locations *)malloc(sizeof(locations));
    locations *tail=result;
    while(lptr1!=NULL && lptr2!=NULL){
        if(compare_locations_by_key_descending(lptr1, lptr2,key) >= 0){
            tail->next=lptr1;
            lptr1=lptr1->next;
        }
        else{
            tail->next=lptr2;
            lptr2=lptr2->next;
        }
        tail = tail->next; 
    }
    if(lptr1!=NULL){
        tail->next=lptr1;
    }
    if(lptr2!=NULL){
        tail->next=lptr2;
    }
    return result->next;
}
void helper_sort_locations_by_violations(locations **head, int key){
    if(*head==NULL || (*head)->next==NULL){
        return;
    }
    locations *second_half=helper_divide_locations(*head);
    helper_sort_locations_by_violations(&second_half, key);
    helper_sort_locations_by_violations(head, key);
    *head=helper_merge_locations_descending(*head,second_half,key);
}


//7

void main_sort_locations_by_violations_descending(){
    helper_sort_locations_by_violations(&location_head, 1);
    helper_print_locations();
}

//8

void main_sort_locations_by_revenue_descending(){
    helper_sort_locations_by_violations(&location_head, 2);
    helper_print_locations();
}


void main_sort_locations_by_violations(){
    helper_sort_locations_by_violations(&location_head, 1);
    helper_print_locations();
}
void main_sort_locations_by_revenue(){
    helper_sort_locations_by_violations(&location_head, 2);
    helper_print_locations();
}

vehicle * helper_divide_vehicles(vehicle *head){
    if(head!=NULL && head->next!=NULL){
        vehicle *ptr1=head;
        vehicle *ptr2=head->next;
        while(ptr2!=NULL && ptr2->next!=NULL){
            ptr1=ptr1->next;
            ptr2=ptr2->next->next;
        }
        vehicle *temp = ptr1->next; 
        ptr1->next=NULL;
        return temp;
    }
    return NULL;
}

int compare_vehicles_descending_by_no_of_unpaid_fines(vehicle *v1, vehicle *v2){
    return ((v2->total_fines_accured-v2->total_fines_paid) - (v1->total_fines_accured-v1->total_fines_paid));
}

//5

void print_owner_violation_for_given_location(int loc_id){
    locations *loc=helper_find_location(loc_id);
    if(loc==NULL){
        printf("Location with ID %d not found.\n", loc_id);
        return;
    }
    violations *ptr=violations_head;
    int found=0;
    while(ptr!=NULL){
        if(ptr->loc==loc){
            printf("Vehicle Reg No: %s,Owner name: %s, Phone no: %lld, Type: %s",
                   ptr->vh->reg_no, ptr->vh->owner_name, ptr->vh->phone_no,
                   type_names[ptr->type_vio].type_name);
            found=1;
        }
        ptr=ptr->next;
    }
    if(!found){
        printf("No violations found for location ID %d.\n", loc_id);
    }
}


//2//


void report_violation(int violation_id, const char *reg_no, int loc_id, type_of_violation type_vio, const char *datetime) {
    vehicle *veh = helper_find_vehicle(reg_no);
    if (veh == NULL) {
        printf("Vehicle with registration number %s not found.\n", reg_no);
        return;
    }

    locations *loc = helper_find_location(loc_id);
    if (loc == NULL) {
        printf("Location with ID %d not found.\n", loc_id);
        return;
    }

    // Calculate fine based on violation type
    float fine_amount = 0.0;
    switch (type_vio) {
        case illegal_parking:
            fine_amount = 100.0;
            break;
        case over_speeding:
            fine_amount = 200.0;
            break;
        case red_light_jump:
            fine_amount = 150.0;
            break;
        case helmet_seatbelt_infraction:
            fine_amount = 50.0;
            break;
        case others:
            fine_amount = 75.0;
            break;
    }

    // Apply surcharge if the vehicle has more than 2 past violations
    if (veh->no_of_violations >= 2) {
        fine_amount *= 1.10;
    }

    // Create a new violation record
    violations *new_violation = (violations *)malloc(sizeof(violations));
    new_violation->violation_id = violation_id;
    new_violation->vh = veh;
    new_violation->loc = loc;
    new_violation->type_vio = type_vio;
    strncpy(new_violation->datetime, datetime, sizeof(new_violation->datetime) - 1);
    new_violation->ispaid = false;
    new_violation->fine_amount = fine_amount;
    new_violation->next = violations_head;
    violations_head = new_violation;

    // Update vehicle's total fines
    veh->total_fines_accured += fine_amount;
    veh->pending_amount += fine_amount;
    veh->no_of_violations += 1;
    veh->no_of_unpaid_violations += 1;

    // Update location's violation count and revenue generated
    loc->no_of_violations += 1;

    printf("Violation reported successfully for vehicle %s at location %d. Fine amount: %.2f\n", reg_no, loc_id, fine_amount);
}
typedef struct {
    type_of_violation type;
    char name[50];
    int count;
}violation_type_frequncy;


void main_display_violations_by_type_frequency() {
    violation_type_frequncy freq[5] = {
        {illegal_parking, "Illegal Parking", 0},
        {over_speeding, "Over Speeding", 0},
        {red_light_jump, "Red Light Jump", 0},
        {helmet_seatbelt_infraction, "Helmet/Seatbelt Infraction", 0},
        {others, "Others", 0}
    };

    violations *ptr = violations_head;
    while (ptr != NULL) {
        freq[ptr->type_vio].count++;
        ptr = ptr->next;
    }
    //sort the freq array in descending order based on count
    for (int i = 0; i < 5 - 1; i++) {
        for (int j = 0; j < 5 - i - 1; j++) {
            if (freq[j].count < freq[j + 1].count) {
                violation_type_frequncy temp = freq[j];
                freq[j] = freq[j + 1];
                freq[j + 1] = temp;
            }
        }
    }
    printf("Violation Type Frequency:\n");
    for (int i = 0; i < 5; i++) {
        printf("%s: %d\n", freq[i].name, freq[i].count);
    }
}

//14


void display_payment_history_reverse_chronological() {
    Payments *ptr = payments_stack_top;
    if (ptr == NULL) {
        printf("No payment history available.\n");
        return;
    }

    printf("Payment History (Reverse Chronological Order):\n");
    while (ptr != NULL) {
        printf("Transaction ID: %d, Violation ID: %d, Registration Number: %s, Amount Paid: %.2f, Payment Mode: %s, Date: %s\n",
               ptr->transaction_id, ptr->violation->violation_id, ptr->reg_no,
               ptr->amount_paid, ptr->payment_mode, ptr->date);
        ptr = ptr->next;
    }
}

// Helper 
typedef struct {
    type_of_violation type;
    char name[50];
    int total_count;
    int repeat_offender_count;
    float fraction;
} violation_fraction_stats;


// 10


void display_violation_types_by_repeat_offender_fraction() {
    violation_fraction_stats stats[5] = {
        {illegal_parking, "Illegal Parking", 0, 0, 0.0},
        {over_speeding, "Over Speeding", 0, 0, 0.0},
        {red_light_jump, "Red Light Jump", 0, 0, 0.0},
        {helmet_seatbelt_infraction, "Helmet/Seatbelt Infraction", 0, 0, 0.0},
        {others, "Others", 0, 0, 0.0}
    };

    //Traverse the violations list to count totals and repeat offender occurrences
    violations *ptr = violations_head;
    while (ptr!=NULL) {
        if(ptr->type_vio>=illegal_parking&& ptr->type_vio <= others) {
            int idx =ptr->type_vio;
            stats[idx].total_count++;
            // Check if the associated vehicle is a repeat offender (> 2 violations)
            if (ptr->vh != NULL && ptr->vh->no_of_violations > 2) {
                stats[idx].repeat_offender_count++;
            }
        }
        ptr = ptr->next;
    }

    // Calculate the fraction for each type
    for (int i = 0; i < 5; i++) {
        if (stats[i].total_count > 0) {
            stats[i].fraction = (float)stats[i].repeat_offender_count / stats[i].total_count;
        } else {
            stats[i].fraction = 0.0;
        }
    }

    //Sort
    for (int i=0;i<4;i++) {
        for (int j=0;j<4-i;j++) {
            if (stats[j].fraction < stats[j+1].fraction) {
                violation_fraction_stats temp = stats[j];
                stats[j] = stats[j+1];
                stats[j+1] = temp;
            }
        }
    }
    //Display
    printf("\n--- Violation Types by Repeat Offender Proportion ---\n");
    for (int i = 0; i < 5; i++) {
        printf("%-30s : %.2f%% (%d out of %d by repeat offenders)\n",stats[i].name,stats[i].fraction*100.0,stats[i].repeat_offender_count,stats[i].total_count);
    }
}

/*
main funcions--
    1.insert_vehicle
    2.report_violation
    3.make_payment
    4.find_vehicles_and_violations
    5.print_owner_violation_for_given_location
    6.display_vehicles_descending_by_unpaid_violations
    7.sort_locations_by_violations_descending
    8.sort_locations_by_revenue_descending
    9.display_violations_by_type_frequency
    10. display_violation_types_by_repeat_offender_fraction
    11.display_vehicle_above_threshold
    12.display_violations_of_vehicle
    13.display_vehicles_descending_by_pending_amount
    14.display_payment_history_reverse_chronological


*/

int main(){
    FILE *file=fopen("locations.txt","r");
    if(file==NULL){
        printf("Error opening locations.txt\n");
        return 1;
    }
    else{
        int loc_id;
        char area[60];
        while(fscanf(file,"%d,%59[^\n]",&loc_id,area)==2){
            locations *new_loc=(locations *)malloc(sizeof(locations));
            new_loc->location_id=loc_id;
            strcpy(new_loc->area,area);
            new_loc->no_of_violations=0;
            new_loc->revenue_generated=0;
            new_loc->next=location_head;
            location_head=new_loc;
        }
        fclose(file);
    }
    FILE *file2 = fopen("vehicles.txt", "r");
    if (file2 == NULL) {
        printf("Error opening vehicles.txt\n");
        return 1;
    } else {
        char reg_no[15], owner_name[30], address[50];
        long long int phone_no;
        
        // Added no_of_unpaid_violations here
        int total_fines_accured, total_fines_paid, pending_amount, no_of_violations, no_of_unpaid_violations;
        
        // Updated to read 9 variables (added ;%d at the end)
        while(fscanf(file2,"%14[^;];%29[^;];%lld;%49[^;];%d;%d;%d;%d;%d\n", 
                     reg_no, owner_name, &phone_no, address, 
                     &total_fines_accured, &total_fines_paid, 
                     &pending_amount, &no_of_violations, &no_of_unpaid_violations) == 9){
            vehicle *new_vehicle = (vehicle *)malloc(sizeof(vehicle));
            strcpy(new_vehicle->reg_no, reg_no);
            strcpy(new_vehicle->owner_name, owner_name);
            new_vehicle->phone_no = phone_no;
            new_vehicle->address = (char *)malloc(strlen(address) + 1);
            strcpy(new_vehicle->address, address);
            new_vehicle->total_fines_accured = total_fines_accured;
            new_vehicle->total_fines_paid = total_fines_paid;
            new_vehicle->pending_amount = pending_amount;
            new_vehicle->no_of_violations = no_of_violations;
            new_vehicle->no_of_unpaid_violations = no_of_unpaid_violations; 
            insert_vehicle_sorted(new_vehicle);
        }
        fclose(file2);
    }  
    bool exit_menu = false;
    
    while(!exit_menu){
        printf("\n======================================================\n");
        printf("       Traffic Violation Management System\n");
        printf("======================================================\n");
        printf("1. Register a New Vehicle\n");
        printf("2. Report a Violation\n");
        printf("3. Process a Payment\n");
        printf("4. Find Vehicles & Violations by Owner Name\n");
        printf("5. List Violations for a Specific Location\n");
        printf("6. Display Vehicles (Descending by Unpaid Violations)\n");
        printf("7. Display Locations (Descending by Total Violations)\n");
        printf("8. Display Locations (Descending by Revenue)\n");
        printf("9. Display Violation Types (By Frequency)\n");
        printf("10. Display Violation Types (By Repeat Offender Fraction)\n");
        printf("11. Display Vehicles Above a Violation Threshold\n");
        printf("12. Display Pending Violations of a Specific Vehicle\n");
        printf("13. Display Vehicles (Descending by Pending Amount)\n");
        printf("14. Display Complete Payment History (Reverse Chronological)\n");
        printf("0. Exit\n");
        printf("======================================================\n");
        
        int choice;
        printf("Enter your choice: ");
        if(scanf("%d", &choice) != 1) {
            while(getchar() != '\n'); // Clear buffer if user types letters
            continue;
        }

        switch(choice){
            case 1: {
                char reg_no[15], owner_name[30], address[100];
                long long int phone_no;
                
                printf("Enter Registration Number: ");
                scanf("%14s", reg_no);
                
                // Clear the input buffer before reading strings with spaces
                while(getchar() != '\n');
                
                printf("Enter Owner Name: ");
                scanf("%29[^\n]", owner_name);
                
                printf("Enter Phone Number: ");
                scanf("%lld", &phone_no);
                
                while(getchar() != '\n');
                
                printf("Enter Address: ");
                scanf("%99[^\n]", address);
                
                vehicle *new_vehicle = (vehicle *)malloc(sizeof(vehicle));
                strcpy(new_vehicle->reg_no, reg_no);
                strcpy(new_vehicle->owner_name, owner_name);
                new_vehicle->phone_no = phone_no;
                new_vehicle->address = (char *)malloc(strlen(address) + 1);
                strcpy(new_vehicle->address, address);
                new_vehicle->total_fines_accured = 0;
                new_vehicle->total_fines_paid = 0;
                new_vehicle->pending_amount = 0;
                new_vehicle->no_of_violations = 0;
                new_vehicle->no_of_unpaid_violations = 0;
                insert_vehicle_sorted(new_vehicle);
                printf("Vehicle registered successfully!\n");
                break;
            }
            case 2: {
                int violation_id, loc_id;
                int type_input;
                char reg_no[15], datetime[20];
                
                printf("Enter Violation ID: ");
                scanf("%d", &violation_id);
                printf("Enter Vehicle Registration Number: ");
                scanf("%14s", reg_no);
                
                printf("\n--- Available Locations ---\n");
                main_print_locations();
                printf("Enter Location ID: ");
                scanf("%d", &loc_id);
                
                printf("Enter Violation Type (0: Illegal Parking, 1: Over Speeding, 2: Red Light Jump, 3: Helmet/Seatbelt, 4: Others): ");
                scanf("%d", &type_input);
                if (type_input < 0 || type_input > 4) {
                    printf("Invalid violation type.\n");
                    break;
                }
                
                while(getchar() != '\n');
                printf("Enter Date and Time (YYYY-MM-DD HH:MM): ");
                scanf("%19[^\n]", datetime);
                
                report_violation(violation_id, reg_no, loc_id, (type_of_violation)type_input, datetime);
                break;
            }
            case 3: {
                int violation_id_payment;
                float amount_paid;
                char payment_mode[20], date[11];
                
                printf("Enter Violation ID for Payment: ");
                scanf("%d", &violation_id_payment);
                printf("Enter Amount Paid: ");
                scanf("%f", &amount_paid);
                
                while(getchar() != '\n');
                printf("Enter Payment Mode (Cash, Card, Online): ");
                scanf("%19[^\n]", payment_mode);
                printf("Enter Payment Date (YYYY-MM-DD): ");
                scanf("%10s", date);
                
                process_payment(violation_id_payment, amount_paid, payment_mode, date);
                break;
            }
            case 4: {
                char owner_name_search[30];
                while(getchar() != '\n');
                printf("Enter Owner Name to Search: ");
                scanf("%29[^\n]", owner_name_search);
                main_find_vehicles_and_violations(owner_name_search);
                break;
            }
            case 5: {
                int loc_id_search;
                printf("\n--- Available Locations ---\n");
                main_print_locations();
                printf("Enter Location ID to Search: ");
                scanf("%d", &loc_id_search);
                print_owner_violation_for_given_location(loc_id_search);
                break;
            }

            case 6:{
                main_display_vehicles_descending_by_unpaid_violations();
                break;
            }

            case 7:{
                main_sort_locations_by_violations_descending();
                break;
            } 

            case 8:{
                main_sort_locations_by_revenue_descending();
                break;
            }  

            case 9:{
                main_display_violations_by_type_frequency();
                break;
            }
               
            case 10:{
                display_violation_types_by_repeat_offender_fraction();
                break;
            }
            case 11: {
                int threshold;
                printf("Enter the threshold number of violations: ");
                scanf("%d", &threshold);
                display_vehicle_above_threshold(threshold);
                break;
            }

            case 12: {
                char search_reg[15];
                printf("Enter Vehicle Registration Number: ");
                scanf("%14s", search_reg);
                display_violations_of_vehicle(search_reg);
                break;
            }

            case 13:{
                main_display_vehicles_descending_by_pending_amount();
                break;
            }

            case 14:{
                display_payment_history_reverse_chronological();
                break;
            }

            case 0:
                exit_menu = true;
                printf("Exiting the Traffic Violation Management System...\n");
                break;
            default:
                printf("Invalid choice. Please enter a number between 0 and 14.\n");
        }
    }    
    
    
    
}
