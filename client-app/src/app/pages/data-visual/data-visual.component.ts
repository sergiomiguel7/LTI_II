import { HttpClient } from '@angular/common/http';
import { Component, OnInit } from '@angular/core';
import { PageEvent } from '@angular/material/paginator';
import { Chart } from 'chart.js';
import * as moment from 'moment';

@Component({
  selector: 'app-data-visual',
  templateUrl: './data-visual.component.html',
  styleUrls: ['./data-visual.component.scss']
})
export class DataVisualComponent implements OnInit {

  totalValues: number = 0;
  limit: number = 10;
  page: number = 1;
  pageSizeOptions: number[] = [10, 25, 50];
  displayedColumns: string[] = ['date', 'value'];

  chartData: any;
  tableData: any = [];
  chart: any;

  showChart: boolean = true;
  showTable: boolean = true;

  constructor(private http: HttpClient) {
  }

  ngOnInit(): void {
    this.getValues();
  }


  getValues(event?: any) {
    let page = event ? event.pageIndex : this.page;
    let limit = event ? event.pageSize : this.limit;

    this.http.get(`http://localhost:5000/api/values?page=${page}&limit=${limit}`).subscribe((data: any) => {
      console.log("data", data);

      let tableData = [] as any;
      this.totalValues = data.storedValues;
      let labels: any[] = [];
      let dataSet: any[] = [];

      data.data.forEach((el: any) => {
        let date = moment(new Date(el.timestamp * 1000)).format("DD/MM/YYYY HH:mm");
        //chart
        labels.push(date);
        dataSet.push(el.value);

        //table
        tableData.push({ date: date, value: el.value });
      });

      this.chartData = {
        data: dataSet,
        labels: labels
      }

      this.tableData = tableData;
      this.buildChart();

    })
  }

  buildChart() {
    var ctx = document.getElementById('myChart') as any;

    if (this.chart !== undefined) {
      this.chart.destroy();
    }

    this.chart = new Chart(ctx, {
      type: 'bar',
      data: {
        labels: this.chartData.labels,
        datasets: [{
          label: 'Valores Lux',
          data: this.chartData.data,
          borderWidth: 1,
          backgroundColor: 'rgba(54, 162, 235, 0.2)',
          borderColor: 'rgb(54, 162, 235)',
        }]
      },
      options: {
        responsive: true,
        maintainAspectRatio: false,
        plugins: {
          datalabels: {
            display: false
          }
        },
      }
    });
  }

  toggle(event: any, type: any) {
    let value = event.checked;
    switch (type) {
      case 'chart': this.showChart = value;
        this.buildChart();
        break;
      case 'table': this.showTable = value;
        break;
    }
  }
}
