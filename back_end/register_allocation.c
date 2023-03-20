#include"register_allocation.h"
struct queue *head;
int *non_available_colors;
int *RIG;
struct variable *list_of_variables;
struct tac_var * _tac_var;
int num_of_nodes;
int edge_num;
int var_num=0;
char *func_name_reg;
int k=6;
int rig_num;
struct var_fist_last * var_f_l;
struct reg_now *reg_now_tac;
//char varnum[1000][40];
int func_start;
int func_end;
int _b_start;
int _b_end;
int _numnum;
int block_num;
int if_neicun=0;
struct name_num
{
    int num;
    char *name;
    int ifparam;
}*live;

struct  edge
{
    int a;
    int b;
}*_bian;

void minimize_RIG()
{
   for(int i = 0; i <rig_num; i++)
   {
       if(list_of_variables[i].neighbor_count < k)
       {
            list_of_variables[i].color = REMOVED;
       }
   }
}

void test_minimize_RIG()
{
    for(int i = 0; i < rig_num; i++)
    {
        if(list_of_variables[i].color == REMOVED)
        {
            printf("variable: %s has been removed\n", list_of_variables[i].name);
        }
    }        
}


void init_non_available_colors()
{
    int temp = k / 32;
    temp += (k % 32) != 0;
    non_available_colors = (int *)malloc(temp * sizeof(int));
}

void reset_non_available_colors()
{
    int i;
    int temp = k / 32;
    temp += (k % 32) != 0;
    for(i = 0; i < temp; i++)
    {
        non_available_colors[i] = 0;
    }
}

int first_fit_coloring()
{
    // printf("???");
    head = NULL;

    for(int i = 0; i < rig_num; i++)
    {
        if(list_of_variables[i].color == NO_COLOR)
        {
            __push(i);
            break;
        }
    }
    while(head != NULL)
    {
        int index = __pop();
        if(visit(index))    return 1;
    }
    return 0;
}

int visit(int index)
{
    int is_removed = 0;
    if(list_of_variables[index].color == REMOVED)
    {
        is_removed = 1;
    }

    for(int i = 0; i < rig_num; i++)
    {
        if(CHECKBIT(index,i))
        {
            if(list_of_variables[i].color > -1)
            {
                SET(list_of_variables[i].color);
                // non_available_colors[list_of_variables[i].color /32] |= (1 << (i % 32) );
            }
            if(list_of_variables[i].color == NO_COLOR && !is_removed)
            {
                __push(i);
            }
        }
    }

    int color = -1; 
    for(int i = 0; i < k; i++)
    {
        if(!CHECK(i))
        //if(!(non_available_colors[i/32] & (1 << (i % 32) )))
        {
            color = i;
            break;
        }
    }

    reset_non_available_colors();

    if(color == -1)
    {
        return 1;
    }
    else
    {
        list_of_variables[index].color = color;
    }
    return 0;
} 

void __push(int variable_index)
{
    struct queue *element = (struct queue *)malloc(sizeof(struct queue));
    element->variable_index = variable_index;
    element->next = NULL;

    if(head == NULL)
    {
        head = element;
    }
    else
    {
        struct queue *temp = head;
        while(temp->next != NULL)
        {
            temp  = temp->next;
        }
        temp->next = element;
    }
}

int __pop()
{
    struct queue *temp = head;
    head = head->next;

    int variable_index = temp->variable_index;
    free(temp);
    return variable_index;
}

void reset_colors()
{
    for(int i = 0; i < rig_num; i++)
    {
        if(list_of_variables[i].color >= 0)
        {
            list_of_variables[i].color = NO_COLOR;
        }
    }
}

int supercmp(char *a ,char *b)
{
    //printf("%s\t%s\n",a,b);
    if(a==NULL||b==NULL)    return -1;
    return strcmp(a,b);
}

void reset_queue()
{
    struct queue *temp = head;
    while(head != NULL)
    {
        head = temp->next;
        free(temp);
        temp = head;
    }
}

void color_removed()
{
    for(int i = 0; i < rig_num; i++)
    {

        if(list_of_variables[i].color == REMOVED)
        {
            if(visit(i))
            {
                //printf("color removed failed");
                //需修改
            } 
        }
    }
}

void print_non_available_colors()
{
   for(int i = 0; i < k; i++)
   {
       printf("%d ", CHECK(i));
   }
}


void print_colors()
{
    int i, j;
    char *name;
    int color;
    for(i = 0; i < rig_num; i++)
    {
        name = list_of_variables[i].name;
        color = list_of_variables[i].color;
        if(color == SPILLED)
        {
            printf("var: %s\t is spilled onto stack\n", name);
        }
        else
        {
            printf("var: %s\tcolor: %d\n", name, color);
        }
    }

    printf("\t");
    for(i = 0; i < rig_num; i++)
    {
        printf("%d ", list_of_variables[i].color);
    }
    printf("\n\n");
    for(i = 0; i < rig_num; i++)
    {
        printf("%d\t", list_of_variables[i].color);
		for(j = 0; j < rig_num; j++)
		{
			int bit = CHECKBIT(i,j);
            if(bit)
                printf("1 ");
            else
                printf("0 ");
		}
		printf("\n");
    }
}


void init_RIG()
{
    list_of_variables=NULL;
    num_of_nodes=rig_num;
    RIG=NULL;
}


void create_RIG()
{
    int size = rig_num;
    int temp = 0;
    size = size * size;
    temp = size % 32 != 0;
    size = size / 32;
    size += temp;
    RIG = (int *)malloc(sizeof(int)*size);
    for(int i=0;i<size;i++) RIG[i]=0;
    //print_RIG();

    create_variable_list();
    for(int i=0;i<edge_num;i++)
    {
        create_edge(_bian[i].a, _bian[i].b);
        //printf("%d-%d\n",_bian[i].a, _bian[i].b);
    }
    // printf("\n\nedgenum:\n\n");
    // for(int i=0;i<rig_num;i++)  printf("%d  %s\t %d\n",i,live[i].name,list_of_variables[i].neighbor_count);
}


void create_edge(int first_node, int second_node)
{
    int i=first_node;
    int j=second_node;
    SETBIT(i,j);
    SETBIT(j,i);
    // RIG[(((i*rig_num)+j)/32)] |= (1 << ((i*rig_num+j)%32));
    // RIG[(((j*rig_num)+i)/32)] |= (1 << ((j*rig_num+i)%32));
    list_of_variables[i].neighbor_count++;
    list_of_variables[j].neighbor_count++;
    //printf("%s:%d\t%s:%d\n",live[i].name,list_of_variables[i].neighbor_count,live[j].name,list_of_variables[j].neighbor_count);
}

int find_var(char * str)
{
    if(str==NULL)   return -1;
    for(int i=0;i<rig_num;i++)
    {
        if(supercmp(live[i].name,str)==0)
            return i;
    }
    return -1;
}

void create_variable_list()
{
    int index = 0;
    list_of_variables = (struct variable *)malloc(rig_num * sizeof(struct variable));
    char *name;
    int len;
    struct variable *temp;
    int i;
    for(i = 0; i < rig_num; i++)
    {
        len=1;
        int j=i;
        while(j)
        {
            j/=10;
            len++;
        }
        name = (char *)malloc(len * sizeof(char));
        sprintf(name, "%d", i);
        list_of_variables[i].name = name;
        // printf("%s\n",list_of_variables[i].name);
        list_of_variables[i].color = NO_COLOR;
        list_of_variables[i].neighbor_count = 0;
    }
    // int index = 0;
    // list_of_variables = (struct variable *)malloc(rig_num * sizeof(struct variable));
    // char *name;
    // struct variable *temp;
    // int i;
    // for(i = 0; i < rig_num; i++)
    // { 
    //     name = (char *)malloc(MAXSTRINGSIZE * sizeof(char));
    //     sprintf(name, "%d", i);
    //     list_of_variables[i].name = name;
    //     list_of_variables[i].color = NO_COLOR;
    //     list_of_variables[i].neighbor_count = 0;
    // }
    // for(int i=0;i<rig_num;i++)
    //     printf("%d %s %d %d\n",i,list_of_variables[i].name,list_of_variables[i].color,list_of_variables[i].neighbor_count);
}


void print_RIG()
{
    int i, j;
    printf("RIG:\n");
    //for(int i=0;i<sizeof(RIG)/4;i++)  printf("%d ",RIG[i]);
    printf("\n");
    for(i = 0; i < rig_num; i++)
    {
		for(j = 0; j < rig_num; j++)
		{
			int bit = CHECKBIT(i,j);
            if(bit)
                printf("1 ");
            else
                printf("0 ");
		}
		printf("\n");
    }
}


void spill_variable()
{
    int i;
    int index_of_max = 0;
    int max = 0;
    for(i = 0; i < rig_num; i++)
    {
        if(list_of_variables[i].neighbor_count > max && list_of_variables[i].color == NO_COLOR)
        {
            max = list_of_variables[i].neighbor_count;
            index_of_max = i;
        }
    }
    list_of_variables[index_of_max].color = SPILLED;

    for(i = 0; i < rig_num; i++)
    {
        if(CHECKBIT(index_of_max,i))
        {
            list_of_variables[i].neighbor_count--;
        }
    }
}    