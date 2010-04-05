;;;
;;; Copyright (c) 2003-2010 uim Project http://code.google.com/p/uim/
;;;
;;; All rights reserved.
;;;
;;; Redistribution and use in source and binary forms, with or without
;;; modification, are permitted provided that the following conditions
;;; are met:
;;; 1. Redistributions of source code must retain the above copyright
;;;    notice, this list of conditions and the following disclaimer.
;;; 2. Redistributions in binary form must reproduce the above copyright
;;;    notice, this list of conditions and the following disclaimer in the
;;;    documentation and/or other materials provided with the distribution.
;;; 3. Neither the name of authors nor the names of its contributors
;;;    may be used to endorse or promote products derived from this software
;;;    without specific prior written permission.
;;;
;;; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS'' AND
;;; ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
;;; IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
;;; ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE
;;; FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
;;; DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
;;; OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
;;; HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
;;; LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
;;; OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
;;; SUCH DAMAGE.
;;;;

;; from GTK
(require "generic.scm")
;;
(define ipa-x-sampa-rule
  '(
((("!" ))("↓"))
((("!" "\\" ))("!"))
((("\"" )) ("ˈ"))
((("%" )) ("ˌ"))
((("&" )) ("ɶ"))
((("'" )) ("ʲ"))
((("-" "\\" )) ("‿"))
((("." )) ("."))
((("/" )) ("/"))
((("1" )) ("ɨ"))
((("2" )) ("ø"))
((("3" )) ("ɜ"))
((("3" "\\" )) ("ɞ"))
((("4" )) ("ɾ"))
((("5" )) ("ɫ"))
((("6" )) ("ɐ"))
((("7" )) ("ɤ"))
((("8" )) ("ɵ"))
((("9" )) ("œ"))
(((":" )) ("ː"))
(((":" "\\" )) ("ˑ"))
((("<" "B" ">" )) ("˩"))
((("<" "F" ">" )) ("↘"))
((("<" "H" ">" )) ("˦"))
((("<" "L" ">" )) ("˨"))
((("<" "M" ">" )) ("˧"))
((("<" "R" ">" )) ("↗"))
((("<" "T" ">" )) ("˥"))
((("<" "\\" )) ("ʢ"))
((("=" )) ("̩"))
((("=" "\\" )) ("ǂ"))
(((">" "\\" )) ("ʡ"))
((("?" )) ("ʔ"))
((("?" "\\" )) ("ʕ"))
((("@" )) ("ə"))
((("@" "\\" )) ("ɘ"))
((("@" "`" )) ("ɚ"))
((("A" )) ("ɑ"))
((("B" )) ("β"))
((("B" "\\" )) ("ʙ"))
((("C" )) ("ç"))
((("D" )) ("ð"))
((("E" )) ("ɛ"))
((("E" "`" )) ("ɝ"))
((("F" )) ("ɱ"))
((("G" )) ("ɣ"))
((("G" "\\" )) ("ɢ"))
((("G" "\\" "_" ">" )) ("ʛ"))
((("H" )) ("ɥ"))
((("H" "\\" )) ("ʜ"))
((("I" )) ("ɪ"))
((("J" )) ("ɲ"))
((("J" "\\" )) ("ɟ"))
((("J" "\\" "_" ">" )) ("ʄ"))
((("K" )) ("ɬ"))
((("K" "\\" )) ("ɮ"))
((("L" )) ("ʎ"))
((("L" "\\" )) ("ʟ"))
((("M" )) ("ɯ"))
((("M" "\\" )) ("ɰ"))
((("N" )) ("ŋ"))
((("N" "\\" )) ("ɴ"))
((("O" )) ("ɔ"))
((("O" "\\" )) ("ʘ"))
((("P" )) ("ʋ"))
((("Q" )) ("ɒ"))
((("R" )) ("ʁ"))
((("R" "\\" )) ("ʀ"))
((("S" )) ("ʃ"))
((("T" )) ("θ"))
((("U" )) ("ʊ"))
((("V" )) ("ʌ"))
((("W" )) ("ʍ"))
((("X" )) ("χ"))
((("X" "\\")) ("ħ"))
((("Y" )) ("ʏ"))
((("Z" )) ("ʒ"))
((("Z" "\\" )) ("ʓ"))
((("[" )) ("[" ))
((("]" )) ("]" ))
((("^" )) ("↑" ))
((("_" "\"" )) ("̈"))
((("_" "+" )) ("̟"))
((("_" "-" )) ("̠"))
((("_" "/" )) ("̌"))
((("_" )) ("̥" ))
((("_" "1" )) ("₁"))
((("_" "2" )) ("₂"))
((("_" "3" )) ("₃"))
((("_" "4" )) ("₄"))
((("_" "5" )) ("₅"))
((("_" "=" )) ("̩"))
((("_" ">" )) ("ʼ"))
((("_" "A" )) ("̘"))
((("_" "B" )) ("̏"))
((("_" "F" )) ("̂"))
((("_" "G" )) ("ˠ"))
((("_" "H" )) ("́"))
((("_" "L" )) ("̀"))
((("_" "M" )) ("̄"))
((("_" "N" )) ("̼"))
((("_" "O" )) ("̹"))
((("_" "R" )) ("̌"))
((("_" "T" )) ("̋"))
((("_" "X" )) ("˘"))
((("_" "?" "\\")) ("ˤ"))
((("_" "\\" )) ("̂"))
((("_" "^" )) ("̯"))
((("_" "a" )) ("̺"))
((("_" "c" )) ("̜"))
((("_" "d" )) ("̪"))
((("_" "e" )) ("̴"))
((("_" "h" )) ("ʰ"))
((("_" "j" )) ("ʲ"))
((("_" "k" )) ("̰"))
((("_" "l" )) ("ϡ"))
((("_" "m" )) ("̻"))
((("_" "n" )) ("ⁿ"))
((("_" "o" )) ("̞"))
((("_" "q" )) ("̙"))
((("_" "r" )) ("̝"))
((("_" "t" )) ("̤"))
((("_" "v" )) ("̬"))
((("_" "w" )) ("ʷ"))
((("_" "x" )) ("̽"))
((("_" "}" )) ("̚"))
((("_" "~" )) ("̃"))
((("`" )) ("˞"))
((("a" )) ("a"))
((("b" )) ("b"))
((("b" "_" "<" )) ("ɓ"))
((("c" )) ("c"))
((("d" )) ("d"))
((("d" "_" "<" )) ("ɗ"))
((("d" "`" )) ("ɖ"))
((("e" )) ("e"))
((("f" )) ("f"))
((("g" )) ("ɡ"))
((("g" "_" "<" )) ("ɠ"))
((("h" )) ("h"))
((("h" "\\" )) ("ɦ"))
((("i" )) ("i"))
((("j" )) ("j"))
((("j" "\\" )) ("ʝ"))
((("k" )) ("k"))
((("l" )) ("l"))
((("l" "\\" )) ("ɺ"))
((("l" "\\" "\\" )) ("ɼ"))
((("l" "`" )) ("ɭ"))
((("m" )) ("m"))
((("n" )) ("n"))
((("n" "`" )) ("ɳ"))
((("o" )) ("o"))
((("p" )) ("p"))
((("p" "\\" )) ("ɸ"))
((("q" )) ("q"))
((("r" )) ("r"))
((("r" "\\" )) ("ɹ"))
((("r" "\\" "`" )) ("ɻ"))
((("r" "`" )) ("ɽ"))
((("s" )) ("s"))
((("s" "\\" )) ("ɕ"))
((("s" "`" )) ("ʂ"))
((("t" )) ("t"))
((("t" "`" )) ("ʈ"))
((("u" )) ("u"))
((("v" )) ("v"))
((("v" "\\" )) ("ʋ"))
((("w" )) ("w"))
((("x" )) ("x"))
((("x" "\\" )) ("ɧ"))
((("y" )) ("y"))
((("z" )) ("z"))
((("z" "\\" )) ("ʑ"))
((("z" "`" )) ("ʐ"))
((("{" )) ("æ"))
((("|" )) ("|"))
((("|" "\\" )) ("ǀ"))
((("|" "\\" "|" "\\" )) ("ǁ"))
((("|" "|" )) ("‖"))
((("}" )) ("ʉ"))
((("~" )) ("̃"))))

(define ipa-x-sampa-init-handler
  (lambda (id im arg)
    (generic-context-new id im ipa-x-sampa-rule #t)))

(generic-register-im
 'ipa-x-sampa
 ""
 "UTF-8"
 (N_ "International Phonetic Alphabet (X-SAMPA)")
 (N_ "International Phonetic Alphabet (X-SAMPA)")
 ipa-x-sampa-init-handler)
