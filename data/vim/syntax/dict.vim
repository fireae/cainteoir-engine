" Vim syntax file
" Language:	Dictionary Entries
" Filenames:    *.dict
" Maintainer:	Reece H. Dunn <msclrhd@gmail.com>
" Last Change:	2014 Aug 21

" Quit when a (custom) syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

syn match	dictSpecialEntry	"_[0-9]x"
syn match	dictSpecialEntry	"_10\^[0-9]*"
syn match	dictSpecialEntry	"_andDD"
syn match	dictSpecialEntry	"_DandDD"

syn region	dictComment		start='#' end='$'
syn region	dictPhonemes		start="/" end="/"
syn region	dictString		start="\"" end="\""
syn region	dictDirective		start='^\.[a-z]' end='$' contains=dictComment,dictString

" Define the default highlighting.
" Only used when an item doesn't have highlighting yet
hi def link dictDirective		PreProc
hi def link dictSpecialEntry		Statement
hi def link dictPhonemes		Constant
hi def link dictComment			Comment
hi def link dictString			String

let b:current_syntax = "dict"
" vim: ts=8
