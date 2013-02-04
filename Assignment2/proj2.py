import os
import sys
import getopt
import shlex
import string
import math

def main(argv):
	try:
		opt_list, args = getopt.getopt(argv,"t:c:p:",["class=", "term="]) #Although we are not taking arguments, getopt always returns two lists, one for arguments and options.  
	except getopt.GetoptError, err:
		print str(err)
		usage()
		sys.exit(2)
	menu(opt_list)

def menu(opt_list):
	if opt_list == []:
		print 'no options selected'
	else:
		try:
			for cur_opt, opt_val in opt_list:
				if cur_opt in ('-p'):
					sel_opt = str(opt_val)
			if sel_opt == '1':
				create_dir(opt_list)
			elif sel_opt == '2':
				great_product(list(read_file('numbers')))
			elif sel_opt == '3':
				total_name_score(read_file('names').replace('"','').split(','))
			elif sel_opt == '4':
				tri_num(read_file('words').replace('"','').split(','))				
			else:
				print "please select a vaild menu option"

		except:
			print 'You broke somthing! please read the usage guide!'
			usage()


def create_dir(opt_list):
	dir_list = ['/assignments', '/examples', '/exams', '/lecture_notes', '/submissions']
	local_path = os.path.abspath(sub_path(opt_list))
	symlnk_web = '/usr/local/classes/eecs/' + sub_path(opt_list) + 'public_html'
	symlnk_hndin = '/usr/local/classes/eecs/' + sub_path(opt_list) + 'handin'

	if os.path.exists(local_path):
		print "path already exisits"
	else:
		for i in dir_list:
			os.makedirs((local_path + i), 0755)
		os.symlink(symlnk_web, 'website')
		os.symlink(symlnk_hndin, 'handin')

def sub_path(opts):
		for opt, opt_val in opts: 	
			if opt in ('-t', '--term'):
				term_name = str(opt_val)
			if opt in ('-c', '--class'):
				class_name = str(opt_val)
		return term_name + '/' + class_name + '/'

def great_product(x):	
	print sorted([map(lambda a,b,c,d,e: int(a)*int(b)*int(c)*int(d)*int(e), x[i],x[i+1],x[i+2],x[i+3],x[i+4]) for i in (range(len(x)-5))])[-1]

def total_name_score(name_list):
	name_list.sort()
	total_score = 0
	for i,name in enumerate(name_list):
		total_score += (word_score(name) * (i+1))
	print total_score

def tri_num(word_list):
	num_of_tri = 0
	for name in word_list:
		n = word_score(name)
		if not (math.sqrt(8*n + 1) - 1) / 2 - int((math.sqrt(8*n + 1) - 1) / 2) > 0:
			num_of_tri += 1
	print num_of_tri
		
def word_score(curr_word):
	word_score = 0
	for i in curr_word:
		word_score += dict_key_val(i)
	return word_score

def dict_key_val(char):
	alpha_dict = dict([(value, i+1) for i, value in enumerate(string.ascii_uppercase)])
	for key, value in alpha_dict.items():
		if key == char:
			return value

def read_file(file_path):
	file = open(file_path, 'r')	
	string = str(file.read())
	file.close()
	return string.replace('\n','')

def usage():
	print 'Usage guide: use [-p] option to select a problem 1-4'
	print 'While selecting -p1 use [--term or -t] AND [--class or -c] to create a ../<term><class> directory'
	
if __name__ == "__main__":
	main(sys.argv[1:])


