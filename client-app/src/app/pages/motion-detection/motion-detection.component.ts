import { HttpClient } from '@angular/common/http';
import { Component, OnInit, ViewChild } from '@angular/core';
import { FormControl } from '@angular/forms';
import { MatSort } from '@angular/material/sort';
import { MatTableDataSource } from '@angular/material/table';
import { Chart } from 'chart.js';
import * as moment from 'moment';
import { ValuesService } from 'src/app/services/values.service';

@Component({
  selector: 'app-motion-detection',
  templateUrl: './motion-detection.component.html',
  styleUrls: ['./motion-detection.component.scss']
})
export class MotionDetectionComponent implements OnInit {

  @ViewChild(MatSort, { static: false }) sort: MatSort | undefined;

  totalValues: number = 0;
  limit: number = 10;
  page: number = 1;
  pageSizeOptions: number[] = [10, 25, 50];
  displayedColumns: string[] = ['date', 'sensor'];

  selectedConcentrador = new FormControl();
  selectedSensor = new FormControl();

  sensores: any;
  idSensor: number | any;

  concentradores: any;
  idConcentrador: number | any;


  tableData: any = [];

  constructor(private http: HttpClient,
    private valuesService: ValuesService) {
  }

  ngOnInit(): void {
    this.getConcentradores();
  }

  getConcentradores() {
    this.valuesService.getConcentradores().subscribe((data: any) => {
      this.concentradores = data.data;
    });
  }

  getSensores(){
    this.valuesService.getSensores(this.idConcentrador).subscribe((data: any) => {
      this.sensores = [];      
      this.sensores = data.data;
      this.getValues();
    });
  }

  /**
   * @param event - on select event
   * change the selected concentrador
   */
  changeItem(event: any) {
    this.idConcentrador = event.id;
    this.idSensor = '';
    this.getSensores();
  }


  /**
   * @param event - on select event
   * change the selected sensor
   */
  selectSensor(event: any){
    this.idSensor = event.id;
    this.getValues();
  }


  /**
   * @param event - pagination event
   * get the values from selected filters
   */
  getValues(event?: any) {
    let page = event ? event.pageIndex + 1 : this.page;
    let limit = event ? event.pageSize : this.limit;
    let query = `?page=${page}&limit=${limit}`;

    this.idSensor ? query+=`&id_sensor=${this.idSensor}` : '';

    this.http.get(`http://localhost:5000/api/values/${this.idConcentrador}/S/${query}`).subscribe((data: any) => {

      let tableData = [] as any;
      this.totalValues = data.storedValues;

      //get data from each sensor
      data.data.forEach((el: any) => {

          let date = moment(new Date(el.timestamp * 1000)).format("DD/MM/YYYY HH:mm");

          //table
          tableData.push({
            date: date,
            sensor: this.sensores.find((sensor:any) => sensor.id == el.id_sensor).area,
          });

      });

      this.tableData = new MatTableDataSource<any>(tableData);
      this.tableData.sort = this.sort;

    })
  }

}
