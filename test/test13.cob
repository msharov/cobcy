        identification division.
            program-id. test-program.
        environment division.

        input-output section.
        file-control.
            select sample-rel-file
                assign to disk
                organization is relative
                access mode is sequential.
        data division.
        file section.
        fd sample-rel-file
            label records are standard
            value of file-id is "relative.dbf".
        01 sample-rel-record.
            05 name    picture is X(25).
            05 address picture is X(25).
            05 aptnum  picture is 9(4).
            05 city    picture is X(15).
            05 zip     picture is 99999B9999.

        working-storage section.
        01 count-var        picture is 999 value is 1.

        procedure division.
            display "This program will write a file with relative organization".
            display "The file will be called 'relative.dbf'".
            open output sample-rel-file.

        write-loop.
            move "Whoever" to name.
            move "Wherever" to address.
            move count-var to aptnum.
            move "I don't know" to city.
            move 837480985 to zip.
            write sample-rel-record.
            add 1 to count-var.
            if count-var is greater than 10
                go to end-loop.
            go to write-loop.
        end-loop.
            close sample-rel-file.
            display "All done.".
            exit program.
