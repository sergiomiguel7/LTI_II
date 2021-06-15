import { Component, Inject, OnInit } from '@angular/core';
import { MatDialogRef, MAT_DIALOG_DATA } from '@angular/material/dialog';
import { ValuesService } from 'src/app/services/values.service';

@Component({
  selector: 'app-edit-concentrador',
  templateUrl: './edit-concentrador.component.html',
  styleUrls: ['./edit-concentrador.component.scss']
})
export class EditConcentradorComponent implements OnInit {

  concentrador: any;

  constructor(public dialogRef: MatDialogRef<EditConcentradorComponent>,
    @Inject(MAT_DIALOG_DATA) public data: any,
    private valueService: ValuesService) { }

  ngOnInit(): void {
    this.concentrador = this.data.item;
    
  }

  submit(){
    this.valueService.patchConcentrador(this.concentrador, this.concentrador.id).subscribe((data) => {
      this.dialogRef.close(true);

    });
  }

  onCloseCancel(answer: boolean) {
    this.dialogRef.close(answer);
  }
}
