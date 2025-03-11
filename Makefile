####### MAKEFILE #######

### Variables

SRCS_PATH		=	srcs/
OBJS_PATH		=	objs/
HEAD_PATH		=	includes/

# Finds every header in the src directory
FILES			=	$(shell find $(SRCS_PATH) -name '*.cpp')

# Finds every header in the include directory
HEAD_FILES		=	$(shell find $(HEAD_PATH) -name '*.hpp')

#Finds every directory and subdirectories in include directory and adds -I
INCLUDES		=	$(addprefix -I, $(shell find $(HEAD_PATH) $(SRCS_PATH) -type d))

OBJS			=	$(patsubst $(SRCS_PATH)%.cpp, $(OBJS_PATH)%.o, $(FILES))
DEPS			=	$(patsubst $(SRCS_PATH)%.cpp, $(OBJS_PATH)%.d, $(FILES))

NAME			=	ircserv

CC				=	c++
RM				=	rm -rf
CFLAGS			=	-Wall -Wextra -Werror -g3 -std=c++98

### Commandes

all:			$(NAME)

$(NAME):		$(OBJS)
				$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o $(NAME)

$(OBJS_PATH)%.o:	$(SRCS_PATH)%.cpp
				@mkdir -p $(dir $@)
				# @echo includes : $(INCLUDES)
				$(CC) $(CFLAGS) -MMD -MP $(INCLUDES) -c $< -o $@

-include $(DEPS)

clean:
				$(RM) $(OBJS_PATH)

fclean:			clean
				$(RM) $(NAME)

re:				fclean all

test: all
				./$(NAME)

.PHONY:			all clean fclean re