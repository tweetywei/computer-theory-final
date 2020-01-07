
function CDCBoard(){
    BaseBoard.call(this, 32); 
    console.log("CDC baseboard");
}
CDCBoard.prototype = new BaseBoard();


function CCBoard(){
    BaseBoard.call(this, 90); 
}
CCBoard.prototype = new BaseBoard();



let cdc = new CDCBoard();
let cc = new CCBoard();

cdc.board_init();
cc.board_init();


function Chess() {
    //let board = new Board();
    var blank_size = 50;
    var border_width = 3;
    var piece_font_size = 35;
    var font_size = 18;
    var bound = 5;
    var bound_top = bound;
    var bound_left = bound + ((blank_size + border_width) * 3);

    var die_top_ply = bound;
    var die_left_ply1 = bound + ((blank_size + border_width) * 1);
    var die_left_ply2 = bound + ((blank_size + border_width) * 9);

    var path_label_top = bound + ((blank_size + border_width) * 1);
    var path_top = bound + ((blank_size + border_width) * 1);
    var path_left = bound + ((blank_size + border_width) * 12);
/*
    var STR_NUM = ["", "１", "２", "３", "４", "５", "６", "７", "８"];
    var EN_NUM = ["", "ａ", "ｂ", "ｃ", "ｄ"];
    var PCE_ID = [
        "E",
        "P", "P", "P", "P", "P", "C", "C", "N", "N", "R", "R", "M", "M", "G", "G", "K",
        "D",
        "p", "p", "p", "p", "p", "c", "c", "n", "n", "r", "r", "m", "m", "g", "g", "k"
    ];

    var LANG_EN = 0,
        LANG_CH = 1;
    var LANG = LANG_CH;
    var PCE_MAP = {
        "E": { en: "", ch: "", clr: "EMPTY" },
        "K": { en: "K", ch: "帥", clr: "RED" },
        "G": { en: "G", ch: "仕", clr: "RED" },
        "M": { en: "M", ch: "相", clr: "RED" },
        "R": { en: "R", ch: "俥", clr: "RED" },
        "N": { en: "N", ch: "傌", clr: "RED" },
        "C": { en: "C", ch: "炮", clr: "RED" },
        "P": { en: "P", ch: "兵", clr: "RED" },
        "D": { en: "", ch: "", clr: "DARK" },
        "k": { en: "k", ch: "將", clr: "BLACK" },
        "g": { en: "g", ch: "士", clr: "BLACK" },
        "m": { en: "m", ch: "象", clr: "BLACK" },
        "r": { en: "r", ch: "車", clr: "BLACK" },
        "n": { en: "n", ch: "馬", clr: "BLACK" },
        "c": { en: "c", ch: "包", clr: "BLACK" },
        "p": { en: "p", ch: "卒", clr: "BLACK" },
    };

    var isFirefox = navigator.userAgent.search("Firefox") > -1;

    function div_style(str, top, left) {
        document.write("<div class=\"" + str + "\"")
        document.write("style=\"")
        document.write("width:" + blank_size + "px;")
        document.write("height:" + blank_size + "px;")
        document.write("top:" + top + "px;")
        document.write("left:" + left + "px;")
    }

    function div_style_size(str, top, left, width, heigh) {
        document.write("<div class=\"" + str + "\"")
        document.write("style=\"")
        document.write("width:" + width + "px;")
        document.write("height:" + heigh + "px;")
        document.write("top:" + top + "px;")
        document.write("left:" + left + "px;")
    }

    function shape(str, top, left) {
        div_style(str, top, left);
        document.write("\">")
        document.write("</div>")
    }

    function shape_xy(str, top, left, col, row) {
        var shape_top = top + (col) * (blank_size + border_width)
        var shape_left = left + (row) * (blank_size + border_width)
        shape(str, shape_top, shape_left);
    }

    function shape_xy_test(str, top, left, col, row, text, size, color) {
        var shape_top = top + (col) * (blank_size + border_width)
        var shape_left = left + (row) * (blank_size + border_width)
        div_style(str, shape_top, shape_left)
        document.write("font-size:" + size + "px;")
        document.write("\">")
        font_style(text, color)
        document.write("</div>")
    }

    function font_style(str, color) {
        document.write("<font color=\"" + color + "\";")


        document.write("\">")

        document.write(str)
        document.write("</font>")
    }

    function piece_xy(str, top, left, y, x, player, text) {
        var shape_top = top + (9 - y) * (blank_size + border_width)
        var shape_left = left + (x) * (blank_size + border_width)
        shape(str, shape_top, shape_left);


        div_style(str, shape_top, shape_left);
        document.write("font-size:" + piece_font_size + "px;")
        document.write("border: 3px none #ccc;")

        document.write("line-height:" + blank_size + "px;")

        if (player == 'BLACK') {
            document.write("background:#DDDDDD;")
            document.write("\">")
            font_style(text, "#444444")
        } else if (player == 'RED') {
            document.write("background:#ebd6d6;")
            document.write("\">")
            font_style(text, "#E63F00")
        } else if (player == 'DARK') {
            document.write("background:#D4D4D4;")
            document.write("\">")
        } else if (player == 'RED_DARK') {
            document.write("background:#e3d4d6;")
            document.write("\">")
            font_style(text, "#cfa5ac")
        } else if (player == 'BLACK_DARK') {
            document.write("background:#e4e2e2;")
            document.write("\">")
            font_style(text, "#9C9C9C")
        }
        document.write("</div>")

    }
*/
    //init
   // var STR_NUM = ["", "１", "２", "３", "４", "５", "６", "７", "８"];
    //var EN_NUM  = ["", "ａ", "ｂ", "ｃ", "ｄ"];
    this.init = function() {
        /*
        board.init();
        for (j = 1; j <= 8; j++) {
            for (i = 1; i <= 4; i++)
                board.shape_xy("blank", bound_top, bound_left, j, i, blank_size, border_width);
        }
        for (j = 1, i = 8; j <= 8; j++, i--)
            board.shape_xy_text("boundary", bound_top, bound_left, j, 0, STR_NUM[i], 20, "#9C9C9C", blank_size, border_width);
        for (i = 1; i <= 4; i++)
            board.shape_xy_text("boundary", bound_top, bound_left, 9, i, EN_NUM[i], 20, "#9C9C9C", blank_size, border_width);

        board.shape_xy("backslash", bound_top, bound_left, 4, 3, blank_size, border_width);
        board.shape_xy("backslash", bound_top, bound_left, 5, 4, blank_size, border_width);
        board.shape_xy("slash", bound_top, bound_left, 4, 4, blank_size, border_width);
        board.shape_xy("slash", bound_top, bound_left, 5, 3, blank_size, border_width);
        */   
    }

    this.piece_init = function() {
        for (j = 1; j <= 8; j++) {
            for (i = 1; i <= 4; i++)
                piece_xy("circle", bound_top, bound_left, j, i, "DARK", "");
        }
    }

    this.die_init = function() {
        var count = 1;
        for (j = 1; j <= 8; j++) {
            for (i = 0; i <= 1; i++) {
                piece_xy("circle", die_top_ply, die_left_ply1, j, i, "RED_DARK", PCE_MAP[PCE_ID[count]].ch);
                count++;
            }
        }
        count++;
        for (j = 1; j <= 8; j++) {
            for (i = 0; i <= 1; i++) {
                piece_xy("circle", die_top_ply, die_left_ply2, j, i, "BLACK_DARK", PCE_MAP[PCE_ID[count]].ch);
                count++;
            }
        }
    }

    this.path_init = function(move, start, end, ply, this_ply) {
        var str = "";
        var width = blank_size * 4;
        var height = blank_size * 8;
        var x = 0,
            y = 0;
        var top = path_top;
        var left = path_left;
        var move_shape = "";
        var move_font_color = "";
        var move_background = "";
        if (start == ply) {

            div_style_size("label", top, left + blank_size / 2, blank_size * 2, blank_size / 2)
            document.write("font-size:" + font_size + "px;")
            document.write("\">")
            font_style("走步", "  #102942")
            document.write("</div>")

            top += bound + ((blank_size + border_width) * 0.5);
            div_style_size("path", top, left, width, height)
            document.write("\">")
        }
        if (ply <= this_ply) {
            y = (ply - 1) * (font_size * 2);
            if (ply % 2 == 1) {
                move_shape = "move1";
                move_background = "background:#e3d4d6;";
                move_font_color = "#B20000";
                x = 0;
            } else {
                move_shape = "move2";
                move_background = "background:#e4e2e2;";
                move_font_color = "#545454";
                x = blank_size;
            }
            div_style_size(move_shape, y, x, blank_size * 2, font_size * 2)
            document.write(move_background)
            document.write("font-size:" + font_size - 3 + "px;")
            document.write("\">")
            font_style(move, move_font_color)
            document.write("</div>")
        }
        if (end == ply) {
            top += font_size * 2;
            document.write("</div>")

            left += bound;
            top += bound * 2 + height;
            div_style_size("left_arrow", top, left, font_size * 2, font_size)
            document.write("cursor:pointer;")
            document.write("\"")
            document.write("onclick=\"{location.href='")
            //document.write(b1)
            document.write("'}\"")
            document.write("></div>")

            top -= bound;
            left += bound * 2 + font_size * 2;
            div_style_size("label", top, left, font_size * 4, font_size * 1.5)
            document.write("font-size:" + font_size + "px;")

            document.write("\">")
            str = this_ply.toString() + "/" + end.toString();
            font_style(str, "   #102942")

            document.write("</div>")

            top += bound;
            left += bound * 2 + font_size * 4;
            div_style_size("right_arrow", top, left, font_size * 2, font_size)
            document.write("cursor:pointer;")
            document.write("\"")
            document.write("onclick=\"{location.href='")
            //document.write(b2)
            document.write("'}\"")
            document.write("></div>")
        }
        var obj = document.getElementsByClassName("path")[0];
        obj.scrollTop = obj.scrollHeight;
    }
    let _this = this;
    this.movelist_init = function(moves, ply, prev_url, next_url) {
        for (var i = 0; i < moves.length; ++i)
            _this.path_init(moves[i], 1, moves.length, i + 1, ply);

        document.getElementsByClassName("left_arrow")[0].onclick = function() { location.href = prev_url; };
        document.getElementsByClassName("right_arrow")[0].onclick = function() { location.href = next_url; };
    }

    function pce(pce) {
        switch (LANG) {
            case LANG_EN:
                return PCE_MAP[pce].en;
            case LANG_CH:
                return PCE_MAP[pce].ch;
        }
    }

    function clr(pce) { return PCE_MAP[pce].clr; }




    this.set_board = function(board) {
        console.log(board);
        for (var i = 0; i < board.length; ++i) {
            if (board[i] == "E") continue;
            piece_xy("circle", bound_top, bound_left, 1 + (i % 8), 1 + parseInt(i / 8), clr(board[i]), pce(board[i]));
        }
    }

    this.set_status = function(status) {
        console.log(status);
        var n = 0;
        var row = 8,
            col = 0;
        for (var p in status) {
            var piece = p;
            var open = status[p].open;
            var dark = status[p].dark;
            var died = status[p].died;
            n = open + dark;
            for (i = 0; i < n; i++) {
                if (dark > 0) {
                    if (clr(piece) == "RED")
                        piece_xy("circle", die_top_ply, die_left_ply1, row, col, "RED_DARK", pce(piece));
                    else
                        piece_xy("circle", die_top_ply, die_left_ply2, row, col, "BLACK_DARK", pce(piece));
                    dark--;
                } else if (died > 0) {
                    if (clr(piece) == "RED")
                        piece_xy("circle", die_top_ply, die_left_ply1, row, col, "RED", pce(piece));
                    else
                        piece_xy("circle", die_top_ply, die_left_ply2, row, col, "BLACK", pce(piece));
                    died--;
                } else if (open > 0) {

                    if (clr(piece) == "RED")
                        piece_xy("circle", die_top_ply, die_left_ply1, row, col, "DARK", pce(piece));
                    else
                        piece_xy("circle", die_top_ply, die_left_ply2, row, col, "DARK", pce(piece));
                    open--;
                }
                col++;
                if (col == 2) {
                    col = 0;
                    --row;
                }
                if (row == 0) row = 8;
            }
        }
    }
}